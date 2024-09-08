#include <iostream> // std::cout
#include <mutex>    // std::mutex
#include <pthread.h> // pthread_create
#include <thread>   // std::thread


// 定义单例类

std::mutex mtx;	// 加锁

class SingleInstance
{

public:
    // 获取单例对象
    static SingleInstance *GetInstance();

    // 释放单例，进程退出时调用
    static void deleteInstance();
	
	// 打印单例地址
    void Print();

private:
	// 将其构造和析构成为私有的, 禁止外部构造和析构
    SingleInstance();
    ~SingleInstance();

    // 将其 拷贝构造 和 赋值构造 成为私有函数, 禁止外部拷贝和赋值
    SingleInstance(const SingleInstance &signal);
    const SingleInstance &operator=(const SingleInstance &signal);

private:
    // 唯一单例对象指针
    static SingleInstance *m_SingleInstance;
};

//初始化静态成员变量
SingleInstance *SingleInstance::m_SingleInstance = NULL;

SingleInstance* SingleInstance::GetInstance()
{
    // 加锁保护，防止多个线程同时创建实例
    std::lock_guard<std::mutex> lock(mtx);
	if (m_SingleInstance == NULL)
	{
	    m_SingleInstance = new (std::nothrow) SingleInstance;  // 没有加锁是线程不安全的，当线程并发时会创建多个实例
        // std::nothrow：标记对象，用于在动态内存分配时指示即使分配失败也不抛出异常，而是返回一个空指针。
	}

    return m_SingleInstance;
}

void SingleInstance::deleteInstance()
{
    std::lock_guard<std::mutex> lock(mtx);  // 加锁保护资源释放
    if (m_SingleInstance)
    {
        delete m_SingleInstance;
        m_SingleInstance = NULL;
    }
}

void SingleInstance::Print()
{
	std::cout << "我的实例内存地址是:" << this << std::endl;
}

SingleInstance::SingleInstance()
{
    std::cout << "构造函数" << std::endl;
}

SingleInstance::~SingleInstance()
{
    std::cout << "析构函数" << std::endl;
}
///////////////////  普通懒汉式实现 -- 线程不安全  //////////////////

// 线程函数
void *PrintHello(void *threadid)
{
    // 主线程与子线程分离，两者相互不干涉，子线程结束同时子线程的资源自动回收
    pthread_detach(pthread_self());	// pthread_self()获取当前线程的线程标识符（TID）

    // 对传入的参数进行强制类型转换，由无类型指针变为整形数指针，然后再读取
    int tid = *((int *)threadid);
	
    {
        std::lock_guard<std::mutex> lock(mtx);	// 保护输出
        std::cout << "Hi, 我是线程 ID:[" << tid << "]" << std::endl;
    }

    // 打印实例地址
    SingleInstance::GetInstance()->Print();

    pthread_exit(NULL);
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
        if (ret)	// ret非零，说明线程创建错误
        {
            std::cout << "Error:无法创建线程," << ret << std::endl;
            exit(-1);	// 退出当前进程的函数，传入0为程序成功执行完毕；传入-1表示程序因某种错误而结束
        }
    }

    // 使用pthread_join来等待所有线程执行完毕
    for (i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);  // 等待每个线程执行完毕
    }

    // 手动释放单实例的资源
    SingleInstance::deleteInstance();
    std::cout << "main() : 结束! " << std::endl;
	
    return 0;
}
