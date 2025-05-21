//
// Created by fsptd on 25-5-21.
//
#include <../ac_nowcoder_rankings_server/ac_nowcoder_rankings_server.h>
namespace ac_nowcoder_rankings_server {
        // 更新竞赛列表的函数
        void ac_nowcoder_rankings_server::List_update() {
                // 检查竞赛队列是否为空
                if (Listen_to_the_competition_queue.size()) {
                        Listen_to_the_competition_Template listen_to_the_competition1;
                        // 锁定竞赛队列互斥锁
                        Listen_to_the_competition_queue_mtx.lock();
                        if (Listen_to_the_competition_queue.size()) {
                               // 获取队列中的第一个竞赛
                               listen_to_the_competition1 = Listen_to_the_competition_queue.front();
                               // 从队列中移除该竞赛
                               Listen_to_the_competition_queue.pop();
                        }
                        // 解锁竞赛队列互斥锁
                        Listen_to_the_competition_queue_mtx.unlock();

                        // 锁定停止监控竞赛的互斥锁
                        Stop_monitoring_the_competition_mtx.lock();
                        // 检查是否在停止监控的竞赛列表中
                        if (Stop_monitoring_the_competition.find(listen_to_the_competition1.contest_id) != Stop_monitoring_the_competition.end()) {
                                // 从停止监控的竞赛列表中移除该竞赛
                                Stop_monitoring_the_competition.erase(listen_to_the_competition1.contest_id);
                                // 解锁停止监控竞赛的互斥锁
                                Stop_monitoring_the_competition_mtx.unlock();
                        } else {
                                // 解锁停止监控竞赛的互斥锁
                                Stop_monitoring_the_competition_mtx.unlock();
                                // 更新竞赛的评估数据
                                Get_Evaluation_Data(listen_to_the_competition1.contest_id, listen_to_the_competition1.cookie);

                                // 锁定竞赛队列互斥锁
                                Listen_to_the_competition_queue_mtx.lock();
                                // 将竞赛重新加入队列
                                Listen_to_the_competition_queue.push(listen_to_the_competition1);
                                // 解锁竞赛队列互斥锁
                                Listen_to_the_competition_queue_mtx.unlock();
                        }
                }
        }
}