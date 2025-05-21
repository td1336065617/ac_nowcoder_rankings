#ifndef THREAD_POOL_HTTP_SERVER_H_INCLUDED
#define THREAD_POOL_HTTP_SERVER_H_INCLUDED

// 基础IO和系统调用相关头文件
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <error.h>
#include <sys/stat.h>

// 网络通信相关头文件
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/sendfile.h>

// C++标准库相关头文件
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <semaphore.h>

// 线程池自定义头文件
#include "Thread_Pool_sem/Thread_Pool_sem.h"
using std::mutex;
using std::condition_variable;
using std::queue;
using std::cout;

namespace Thread_Pool {
    // 任务类，用于封装需要执行的任务函数
    class Task {
    private:
        std::function<void()> func_; // 使用std::function封装无参任务
    public:
        // 构造函数，接受任意可调用对象（如函数、lambda等）
        Task(std::function<void()> func) {
            func_=func;
        }
        // 重载()操作符，使任务对象可以像函数一样被调用
        void operator()() {
            func_();
        }

        // 执行任务的成员函数
        void run() {
            func_();
        }
    };
    // 线程池类定义
    class Thread_Pool {
    private:
        int xiaohuibiaoji = false; // 标记是否需要销毁线程池
        int maxcount = 0; // 最大线程数（系统能创建的最大线程数量）
        int mincount = 0; // 最小维持线程数（保持活跃状态的最小线程数量）
        int existing_count = 0; // 当前存在的工作线程数
        int wait_count = 0; // 正在等待的任务数量
        int wait_time = 0; // 等待任务的超时时间
        int addThread_pool(); // 内部方法：增加新的线程到线程池
        Thread_Pool_sem thread_pool_sem; // 信号量控制线程池资源访问
        static void *task_Thread(void *arg); // 静态方法：线程执行任务的入口点
        // 多个互斥锁用于保护共享资源访问
        mutex task_mtx, existing_count_mtx, wait_count_mtx, task_conn_mtx, task_queue_task;
        condition_variable task_conn; // 条件变量用于协调任务连接和处理
        queue<Task> task_queue; // 存储待执行任务的队列
    public:
        int addtanks(Task task); // 添加新任务到任务队列
        void setxiaohui(int xiaohuibiaoji) { this->xiaohuibiaoji = xiaohuibiaoji; } // 设置线程销毁标记
        int get_existing_count() { return existing_count; } // 获取当前运行中的线程数量
        // 初始化信号量以激活线程池
        void thread_pool() {
            for (int i = 0; i < existing_count; i++)thread_pool_sem.addsem();
        }

        Thread_Pool() {
        }; // 默认构造函数初始化线程池

        // 配置线程池参数：最大线程数、最小线程数、等待超时时间
        void set_Thread_Pool(int maxcount, int mincount, int waittime);

        ~Thread_Pool(); // 析构函数负责清理所有资源及终止线程
    };
}

#endif // THREAD_POOL_HTTP_SERVER_H_INCLUDED
