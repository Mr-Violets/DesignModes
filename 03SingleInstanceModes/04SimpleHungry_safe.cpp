#include <iostream> // std::cout
#include <mutex>    // std::mutex
#include <pthread.h> // pthread_create

////////////////////////// 饿汉实现 /////////////////////
class Singleton
{
public:
    // 获取单实例
    static Singleton* GetInstance();

    // 释放单实例，进程退出时调用
    static void deleteInstance();
    
    // 打印实例地址
    void Print();

private:
    // 将其构造和析构成为私有的, 禁止外部构造和析构
    Singleton();
    ~Singleton();

    // 将其拷贝构造和赋值构造成为私有函数, 禁止外部拷贝和赋值
    Singleton(const Singleton &signal);
    const Singleton &operator=(const Singleton &signal);

private:
    // 唯一单实例对象指针
    static Singleton *g_pSingleton;
public:
    static std::mutex coutMutex;
};

std::mutex Singleton::coutMutex;    // 声明互斥锁

// 代码一运行就初始化创建实例 ，本身就线程安全
Singleton* Singleton::g_pSingleton = new (std::nothrow) Singleton;

Singleton* Singleton::GetInstance()
{
    return g_pSingleton;
}

void Singleton::deleteInstance()
{
    if (g_pSingleton)
    {
        delete g_pSingleton;
        g_pSingleton = NULL;
    }
}

void Singleton::Print()
{
    std::cout << "我的实例内存地址是:" << this << std::endl;
}

Singleton::Singleton()
{
    std::cout << "构造函数" << std::endl;
}

Singleton::~Singleton()
{
    std::cout << "析构函数" << std::endl;
}
////////////////////////// 饿汉实现 /////////////////////

// 线程函数
void *PrintHello(void *threadid)
{
    // 对传入的参数进行 强制类型转换，由 无类型指针 变为 整形数指针，然后再读取
    int tid = *((int *)threadid);

    // 保护输出，避免多个线程的输出内容混合在一起
    {
        std::lock_guard<std::mutex> guard(Singleton::coutMutex);
        std::cout << "Hi, 我是线程 ID:[" << tid << "]" << std::endl;
    }

    // 打印实例地址
    Singleton::GetInstance()->Print();

    pthread_exit(NULL); // 确保线程结束
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

    std::cout << "main() : 结束! " << std::endl;

    return 0;
}
