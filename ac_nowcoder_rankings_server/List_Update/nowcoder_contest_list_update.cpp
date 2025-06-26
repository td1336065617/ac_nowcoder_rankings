//
// Created by fsptd on 25-5-21.
//
#include "../ac_nowcoder_rankings_server/ac_nowcoder_rankings_server.h"

namespace ac_nowcoder_rankings_server {
    /**
     * @brief 更新牛客比赛排行榜数据
     * @param contestId 比赛ID
     * @return 返回操作状态码(1表示成功)
     *
     * 该函数负责处理比赛提交记录并更新排行榜数据，主要功能包括：
     * 1. 锁定比赛相关数据结构的互斥锁确保线程安全
     * 2. 获取当前比赛的最大提交ID和比赛模板信息
     * 3. 遍历评估记录并更新用户排名数据
     * 4. 处理不同类型的状态信息(AC/非AC)
     * 5. 计算AC时间和罚时
     * 6. 更新题目统计信息(提交次数、AC次数等)
     * 7. 处理首杀标记
     * 8. 维护比赛的线程安全数据结构

     * 注意:
     * - 使用互斥锁保护所有共享数据的访问
     * - 处理CE和系统错误等不影响排名的提交
     * - 计算罚时规则: 每次错误提交增加20分钟罚时
     * - 自动初始化新用户的数据结构
     * - 维护题目AC状态和首杀标记
     */
    int ac_nowcoder_rankings_server::nowcoder_contest_list_update(long long int contestId) {
        nowcoder_contest_map_mtx[contestId].lock();
        nowcoder_contest_set_mtx[contestId].lock();

        // 锁定最大提交ID的互斥锁，确保线程安全
        nowcoder_contest_list_max_submissionId_mtx.lock();
        // 如果当前比赛的最大提交ID不存在，则初始化为0
        if (nowcoder_contest_list_max_submissionId.find(contestId) == nowcoder_contest_list_max_submissionId.end()) {
            nowcoder_contest_list_max_submissionId[contestId] = 0;
        }
        // 获取当前比赛已经更新入比赛的最大提交ID
        int max_submissionId = nowcoder_contest_list_max_submissionId[contestId];
        // 解锁最大提交ID的互斥锁
        nowcoder_contest_list_max_submissionId_mtx.unlock();

        // 锁定比赛信息映射的互斥锁，确保线程安全
        Contest_Info_map_mtx.lock();
        // 获取当前比赛的模板信息
        Contest_Info_Template contest_info_template = Contest_Info_map[contestId];
        // 解锁比赛信息映射的互斥锁
        Contest_Info_map_mtx.unlock();


        // 遍历当前比赛的评估记录
        for (auto &x: Memorize_the_assessment_records[contestId]) {
            if (x.first == max_submissionId) {
                break;
            }
            // 如果当前记录的提交时间大于比赛结束时间，则跳过该记录
            if (x.second.get_submit_time() >= contest_info_template.contestEndTime) {
                continue;
            }
            update_ac_nowcoder_ranking_data(contestId, contest_info_template, x.second, 0);
        }
        for (auto &x: Memorize_the_assessment_records_Supplementary_order[contestId]) {
            if (x.first) {
                update_ac_nowcoder_ranking_data(contestId, contest_info_template,
                                                Memorize_the_assessment_records[contestId][x.first], 1);
            }
        }
        nowcoder_contest_set_mtx[contestId].unlock();
        nowcoder_contest_map_mtx[contestId].unlock();
        return 1;
    }
}
