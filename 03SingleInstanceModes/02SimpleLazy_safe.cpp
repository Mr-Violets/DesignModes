#include <iostream> // std::cout
#include <mutex>    // std::mutex
#include <pthread.h> // pthread_create

///////////////////  加锁的懒汉式实现  //////////////////
class SingleInstance
{
public:
    // 获取单实例对象
    static SingleInstance *&GetInstance();

    // 释放单实例，进程退出时调用
    static void deleteInstance();
	
    // 打印实例地址
    void Print();

private:
    // 将其构造和析构成为私有的, 禁止外部构造和析构
    SingleInstance();
    ~SingleInstance();

    // 将其拷贝构造和赋值构造成为私有函数, 禁止外部拷贝和赋值
    SingleInstance(const SingleInstance &signal);
    const SingleInstance &operator=(const SingleInstance &signal);

private:
    // 唯一单实例对象指针
    static SingleInstance *m_SingleInstance;
    static std::mutex m_Mutex;
public:
    static std::mutex coutMutex; // 用于保护std::cout
};

// 初始化静态成员变量
SingleInstance *SingleInstance::m_SingleInstance = NULL;
std::mutex SingleInstance::m_Mutex;
std::mutex SingleInstance::coutMutex; // 用于保护std::cout

SingleInstance *&SingleInstance::GetInstance()
{
    // 双检锁机制：避免每次调用 GetInstance 方法都加锁，提高性能
    if (m_SingleInstance == NULL)   // 本质就是判断是否已经初始化过，是否存在已经实例化对象
    {
        std::unique_lock<std::mutex> lock(m_Mutex); // 加锁
        if (m_SingleInstance == NULL)
        {
            m_SingleInstance = new (std::nothrow) SingleInstance;   // nothrow 内存分配不成功，返回NULL，不报错
        }
    }
    return m_SingleInstance;
}

void SingleInstance::deleteInstance()
{
    std::unique_lock<std::mutex> lock(m_Mutex); // 加锁
    if (m_SingleInstance)
    {
        delete m_SingleInstance;
        m_SingleInstance = NULL;
    }
}

void SingleInstance::Print()
{
    // 使用coutMutex保护std::cout的输出，避免多线程同时输出导致错乱
    std::lock_guard<std::mutex> guard(coutMutex);
    std::cout << "我的实例内存地址是:" << this << std::endl;
}

SingleInstance::SingleInstance()
{
    std::lock_guard<std::mutex> guard(coutMutex);
    std::cout << "构造函数" << std::endl;
}

SingleInstance::~SingleInstance()
{
    std::lock_guard<std::mutex> guard(coutMutex);
    std::cout << "析构函数" << std::endl;
}

///////////////////  加锁的懒汉式实现  //////////////////

// 线程函数
void *PrintHello(void *threadid)
{
    // 主线程与子线程分离，两者相互不干涉，子线程结束同时子线程的资源自动回收
    pthread_detach(pthread_self());

    // 对传入的参数进行强制类型转换，由无类型指针变为整形数指针，然后再读取
    int tid = *((int *)threadid);

    // 保护输出，避免多个线程的输出内容混合在一起
    {
        std::lock_guard<std::mutex> guard(SingleInstance::coutMutex);
        std::cout << "Hi, 我是线程 ID:[" << tid << "]" << std::endl;
    }

    // 打印实例地址
    SingleInstance::GetInstance()->Print();

    pthread_exit(NULL);
    return nullptr; // 确保有返回值
}

#define NUM_THREADS 5 // 线程个数

int main(void)
{
    pthread_t threads[NUM_THREADS] = {0};
    int indexes[NUM_THREADS] = {0}; // 用数组来保存i的值

    int ret = 0;
    int i = 0;

    std::cout << "main() : 开始 ... " << std::endl;

    for (i = 0; i < NUM_THREADS; i++)
    {
        std::cout << "main() : 创建线程:[" << i << "]" << std::endl;
        
        indexes[i] = i; //先保存i的值
		
        // 传入的时候必须强制转换为void* 类型，即无类型指针
        ret = pthread_create(&threads[i], NULL, PrintHello, (void *)&(indexes[i]));
        if (ret)
        {
            std::cout << "Error: 无法创建线程, " << ret << std::endl;
            exit(-1);
        }
    }

    // 等待所有线程完成后再销毁单例对象
    for (i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);  // 等待每个线程执行完毕
    }

    // 手动释放单实例的资源
    SingleInstance::deleteInstance();
    std::cout << "main() : 结束! " << std::endl;

    return 0;
}
