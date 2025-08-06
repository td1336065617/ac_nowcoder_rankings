//
// Created by fsptd on 25-5-21.
//
#include <../ac_nowcoder_rankings_server/ac_nowcoder_rankings_server.h>

namespace ac_nowcoder_rankings_server {
    /**
     * @brief 列表更新分发中心的主处理函数
     *
     * 该函数负责持续监听竞赛队列，并将更新任务分发到线程池中处理。
     * 主要流程：
     * 1. 持续运行直到接收到关闭信号(Shutdown_bj)
     * 2. 获取当前竞赛队列(Listen_to_the_competition_queue)的大小
     * 3. 为队列中的每个竞赛创建列表更新任务
     * 4. 将任务提交到线程池(ac_nowcoder_rankings_server_thread_pool)
     * 5. 每次处理完队列后休眠5秒，避免频繁轮询
     */
    void ac_nowcoder_rankings_server::List_Update_Distribution_Center() {
        // 主循环：持续运行直到接收到关闭信号
        while (!Shutdown_bj) {
            try {
                // 获取当前待处理的竞赛数量
                int size = Listen_to_the_competition_queue.size();

                // 处理队列中的每个竞赛
                while (size) {
                    size--;

                    // 创建列表更新任务：调用List_update()方法
                    Thread_Pool::Task task1([this]() {
                        try {
                            this->List_update();
                        } catch (const std::exception& e) {
                            // 捕获并处理List_update中可能抛出的异常
                            // 可以记录日志或进行其他错误处理
                        }
                    });

                    // 将任务提交到线程池进行处理
                    ac_nowcoder_rankings_server_thread_pool.addtanks(task1);
                }
            } catch (const std::exception& e) {
                // 捕获并处理队列操作中可能抛出的异常
                // 可以记录日志或进行其他错误处理
            }

            // 处理间隔：避免CPU过度占用
            sleep(5);
        }
    }
}
