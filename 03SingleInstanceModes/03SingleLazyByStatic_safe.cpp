#include <iostream> // std::cout
#include <mutex>    // std::mutex
#include <pthread.h> // pthread_create

///////////////////  内部静态变量的懒汉实现  //////////////////
class Single
{
public:
    // 获取单实例对象
    static Single &GetInstance();
	
	// 打印实例地址
    void Print();

private:
    // 禁止外部构造
    Single();

    // 禁止外部析构
    ~Single();

    // 禁止外部复制构造
    Single(const Single &signal) = delete;

    // 禁止外部赋值操作
    const Single &operator=(const Single &signal) = delete;

public:
    static std::mutex coutMutex; // 保护输出
};

std::mutex Single::coutMutex;

Single &Single::GetInstance()
{
    // 局部静态特性的方式实现单实例
    static Single signal;
    return signal;
}

void Single::Print()
{
    std::lock_guard<std::mutex> guard(coutMutex); // 确保打印线程安全
    std::cout << "我的实例内存地址是: " << this << std::endl;
}

Single::Single()
{
    std::cout << "构造函数" << std::endl;
}

Single::~Single()
{
    std::cout << "析构函数" << std::endl;
}

///////////////////  内部静态变量的懒汉实现  //////////////////

// 线程函数
void *PrintHello(void *threadid)
{
    // 对传入的参数进行 强制类型转换，由 无类型指针 变为 整形数指针，然后再读取
    int tid = *((int *)threadid);

    // 保护输出，避免多个线程的输出内容混合在一起
    {
        std::lock_guard<std::mutex> guard(Single::coutMutex);
        std::cout << "Hi, 我是线程 ID:[" << tid << "]" << std::endl;
    }

    // 打印实例地址
    Single::GetInstance().Print();

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
