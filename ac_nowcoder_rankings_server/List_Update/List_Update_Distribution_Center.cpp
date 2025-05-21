//
// Created by fsptd on 25-5-21.
//
#include <../ac_nowcoder_rankings_server/ac_nowcoder_rankings_server.h>

namespace ac_nowcoder_rankings_server {
    // 列表更新分发中心的主函数
    void ac_nowcoder_rankings_server::List_Update_Distribution_Center() {
        // 持续运行直到接收到关闭信号
        while (!Shutdown_bj) {
            // 获取当前竞赛队列的大小
            int size = Listen_to_the_competition_queue.size();
            // 处理队列中的每个任务
            while (size) {
                size--;
                // 创建一个任务，用于更新列表
                Thread_Pool::Task task1([this]() {
                    this->List_update();
                });
                // 将任务添加到线程池中
                ac_nowcoder_rankings_server_thread_pool.addtanks(task1);
            }
            // 每次处理完队列后休眠5秒
            sleep(5);
        }
    }
}
