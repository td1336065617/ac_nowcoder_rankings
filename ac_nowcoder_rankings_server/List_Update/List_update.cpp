//
// Created by fsptd on 25-5-21.
//
#include <../ac_nowcoder_rankings_server/ac_nowcoder_rankings_server.h>

namespace ac_nowcoder_rankings_server {
    // 更新竞赛列表的主函数
    // 1. 处理竞赛队列中的竞赛
    // 2. 检查是否需要停止监控竞赛
    // 3. 更新竞赛评估数据
    // 4. 更新竞赛列表并重新入队
    void ac_nowcoder_rankings_server::List_update() {
        // 如果竞赛队列不为空则进行处理
        if (Listen_to_the_competition_queue.size()) {
            Listen_to_the_competition_Template listen_to_the_competition1;

            // 线程安全操作：获取队列中的竞赛
            Listen_to_the_competition_queue_mtx.lock();
            if (Listen_to_the_competition_queue.size()) {
                // 获取队列首元素
                listen_to_the_competition1 = Listen_to_the_competition_queue.front();
                // 弹出队列首元素
                Listen_to_the_competition_queue.pop();
            }
            Listen_to_the_competition_queue_mtx.unlock();

            // 检查是否在停止监控列表中
            Stop_monitoring_the_competition_mtx.lock();
            if (Stop_monitoring_the_competition.find(listen_to_the_competition1.contest_id) !=
                Stop_monitoring_the_competition.end()) {
                // 如果竞赛在停止监控列表中，则移除并解锁
                Stop_monitoring_the_competition.erase(listen_to_the_competition1.contest_id);
                Stop_monitoring_the_competition_mtx.unlock();
            } else {
                Stop_monitoring_the_competition_mtx.unlock();

                // 获取竞赛信息
                long long int contestId = listen_to_the_competition1.contest_id;
                Contest_Info_map_mtx.lock();
                Contest_Info_Template contest_info_template = Contest_Info_map[contestId];
                Contest_Info_map_mtx.unlock();

                // 检查竞赛是否满足停止监控条件：
                // 1. 竞赛结束时间超过1分钟
                // 2. 没有提交记录
                // 3. 没有补充评估记录
                if (contest_info_template.contestEndTime+60*1000 <= time(nullptr)*1000 &&
                    Time_of_sealing_Submit_Map[contestId].size() == 0 &&
                    Memorize_the_assessment_records_Supplementary_order[contestId].size() == 0) {
                    Stop_monitoring_the_competition[contestId]=1;
                }

                // 如果竞赛未结束或有补充评估记录，则获取评估数据
                if (contest_info_template.contestEndTime > time(nullptr)*1000 ||
                    Memorize_the_assessment_records_Supplementary_order[contestId].size()) {
                    Get_Evaluation_Data(listen_to_the_competition1.contest_id, listen_to_the_competition1.cookie);
                }

                // 更新竞赛列表
                nowcoder_contest_list_update(listen_to_the_competition1.contest_id);

                // 线程安全操作：将竞赛重新加入队列尾部
                Listen_to_the_competition_queue_mtx.lock();
                Listen_to_the_competition_queue.push(listen_to_the_competition1);
                Listen_to_the_competition_queue_mtx.unlock();
            }
        }
    }
}
