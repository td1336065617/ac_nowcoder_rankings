//
// Created by fspst on 23-5-7.
//
#include "Thread_Pool_sem.h"

// 构造函数：初始化信号量，初始值为0
Thread_Pool_sem::Thread_Pool_sem()
{
    try
    {
        sem_init(&thread_pool_sem, 0, 0); // 初始化无共享（线程间共享）的信号量，初始值为0
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n'; // 捕获并打印异常信息
    }
}

// 析构函数：销毁信号量
Thread_Pool_sem::~Thread_Pool_sem()
{
    try
    {
        sem_destroy(&thread_pool_sem); // 销毁信号量资源
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n'; // 捕获并打印异常信息
    }
}

// 增加信号量值（释放/通知操作）
int Thread_Pool_sem::addsem()
{
    return sem_post(&thread_pool_sem); // 返回0表示成功，-1表示失败
};

// 带超时等待的信号量P操作（尝试获取资源）
int Thread_Pool_sem::waitsem(timespec wait_time)
{
    return sem_timedwait(&thread_pool_sem, &wait_time); // 超时返回-1，成功返回0
};

// 无限等待的信号量P操作（阻塞直到有资源可用）
int Thread_Pool_sem::waitsem()
{
    return sem_wait(&thread_pool_sem); // 阻塞直到信号量大于0，返回0表示成功
};

// 减少信号量值（与waitsem功能相同，语义上用于不同的理解方式）
int Thread_Pool_sem::reducesem()
{
    return sem_wait(&thread_pool_sem); // 同样是P操作
}

// 获取当前信号量的值
int  Thread_Pool_sem::getsem()
{
    int i;
    sem_getvalue(&thread_pool_sem, &i); // 获取当前信号量的计数值
    return i; // 返回信号量当前值
};
