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
     *
     * 详细说明:
     * - 函数首先锁定比赛相关的两个互斥锁(map和set)，确保数据结构的线程安全
     * - 获取并更新最大提交ID，用于确定需要处理的新提交记录范围
     * - 获取比赛模板信息(如结束时间等)
     * - 遍历评估记录Memorize_the_assessment_records，处理所有新提交:
     *   * 跳过已经处理过的记录(<=max_submissionId)
     *   * 跳过比赛结束后的提交
     *   * 调用update_ac_nowcoder_ranking_data更新排名数据
     * - 遍历补充记录Memorize_the_assessment_records_Supplementary_order:
     *   * 处理需要特殊处理的提交记录
     *   * 调用update_ac_nowcoder_ranking_data更新排名数据(标记为补充处理)
     * - 最后解锁所有互斥锁并返回成功状态
     *
     * 注意:
     * - 使用互斥锁保护所有共享数据的访问
     * - 处理CE和系统错误等不影响排名的提交
     * - 计算罚时规则: 每次错误提交增加20分钟罚时
     * - 自动初始化新用户的数据结构
     * - 维护题目AC状态和首杀标记
     */
    int ac_nowcoder_rankings_server::nowcoder_contest_list_update(long long int contestId) {
        // 锁定比赛map和set的互斥锁，确保线程安全
        // nowcoder_contest_map_mtx: 保护比赛用户排名数据的互斥锁
        // nowcoder_contest_set_mtx: 保护比赛题目统计数据的互斥锁
        nowcoder_contest_map_mtx[contestId].lock();
        nowcoder_contest_set_mtx[contestId].lock();

        // 锁定最大提交ID的互斥锁，确保线程安全
        // nowcoder_contest_list_max_submissionId_mtx: 保护最大提交ID的全局互斥锁
        nowcoder_contest_list_max_submissionId_mtx.lock();
        // 如果当前比赛的最大提交ID不存在，则初始化为0
        if (nowcoder_contest_list_max_submissionId.find(contestId) == nowcoder_contest_list_max_submissionId.end()) {
            nowcoder_contest_list_max_submissionId[contestId] = 0;
        }
        // 获取当前比赛已经更新入比赛的最大提交ID
        // max_submissionId: 记录当前比赛已处理的最新提交ID
        int max_submissionId = nowcoder_contest_list_max_submissionId[contestId];
        // 解锁最大提交ID的互斥锁
        nowcoder_contest_list_max_submissionId_mtx.unlock();

        // 锁定比赛信息映射的互斥锁，确保线程安全
        // Contest_Info_map_mtx: 保护比赛模板信息的互斥锁
        Contest_Info_map_mtx.lock();
        // 获取当前比赛的模板信息(包含比赛开始/结束时间等)
        // contest_info_template: 包含比赛基本信息(开始时间、结束时间等)
        Contest_Info_Template contest_info_template = Contest_Info_map[contestId];
        // 解锁比赛信息映射的互斥锁
        Contest_Info_map_mtx.unlock();

        // 遍历主评估记录，处理所有新提交记录
        // Memorize_the_assessment_records: 存储比赛的所有提交记录
        for (auto &x: Memorize_the_assessment_records[contestId]) {
            // 如果到达已处理的最大提交ID则停止处理
            // x.first: 提交记录的ID
            if (x.first == max_submissionId) {
                break;
            }
            // 如果提交时间超过比赛结束时间，则跳过不处理
            // x.second.get_submit_time(): 获取提交时间
            if (x.second.get_submit_time() >= contest_info_template.contestEndTime) {
                continue;
            }
            // 更新排名数据(0表示常规处理)
            // update_ac_nowcoder_ranking_data: 更新排名数据的核心函数
            update_ac_nowcoder_ranking_data(contestId, contest_info_template, x.second, 0);
        }

        // 存储已完成补充处理的提交ID
        // budanwancheng: 临时存储已完成补充处理的提交ID
        vector<long long int> budanwancheng;

        // 遍历补充评估记录，处理需要特殊处理的提交
        // Memorize_the_assessment_records_Supplementary_order: 存储需要特殊处理的提交记录
        for (auto &x: Memorize_the_assessment_records_Supplementary_order[contestId]) {
            if (x.first) {
                // 更新排名数据(1表示补充处理)
                update_ac_nowcoder_ranking_data(contestId, contest_info_template,
                                                Memorize_the_assessment_records[contestId][x.first], 1);
                // 记录已处理的补充提交ID
                budanwancheng.push_back(x.first);
            }
        }
        // 清理已处理的补充记录
        for (auto x: budanwancheng) {
            Memorize_the_assessment_records_Supplementary_order.erase(x);
        }

        // 解锁比赛set和map的互斥锁
        nowcoder_contest_set_mtx[contestId].unlock();
        nowcoder_contest_map_mtx[contestId].unlock();
        return 1;  // 返回操作成功状态码
    }
}
