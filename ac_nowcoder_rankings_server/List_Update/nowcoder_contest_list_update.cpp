//
// Created by fsptd on 25-5-21.
//
#include "../ac_nowcoder_rankings_server/ac_nowcoder_rankings_server.h"

namespace ac_nowcoder_rankings_server {
    /**
     * @brief 更新牛客比赛排行榜数据
     * @param contestId 比赛ID
     * @return 返回操作状态码(1表示成功，2表示比赛已结束)
     *
     * 该函数负责处理比赛提交记录并更新排行榜数据，主要功能包括：
     * 1. 线程安全地访问和修改比赛相关数据结构
     * 2. 处理常规提交记录和补充提交记录
     * 3. 更新用户排名和题目统计信息
     * 4. 维护比赛的AC状态和首杀标记
     * 5. 处理封存记录的解封逻辑
     *
     * 详细流程:
     * - 锁定比赛map、set和封存提交记录的互斥锁
     * - 获取比赛模板信息(包含开始/结束时间/解封时间等)
     * - 检查比赛是否结束且无补充记录，若是则直接返回
     * - 获取并更新比赛的最大提交ID
     * - 主评估记录处理:
     *   * 跳过已处理的记录和比赛结束后的提交
     *   * 调用update_ac_nowcoder_ranking_data更新排名(模式0)
     * - 补充评估记录处理:
     *   * 检查提交时间是否有效
     *   * 调用update_ac_nowcoder_ranking_data更新排名(模式1)
     *   * 标记无效记录以便清理
     * - 清理已处理的补充记录
     * - 解封榜单处理:
     *   * 检查解封条件(封存状态、无补充记录、解封时间到达)
     *   * 处理封存记录(模式2)
     *   * 清除封存记录
     * - 解锁所有互斥锁并返回状态码
     *
     * 注意事项:
     * - 所有共享数据访问都通过互斥锁保护
     * - 比赛结束后仍会处理补充记录和封存记录
     * - 三种处理模式:
     *   * 0: 主评估记录
     *   * 1: 补充评估记录
     *   * 2: 解封记录
     * - 罚时规则: 每次错误提交增加20分钟
     * - 自动初始化新用户数据结构
     * - 返回状态码说明:
     *   * 1: 处理成功
     *   * 2: 比赛已结束且无补充记录
     */
    int ac_nowcoder_rankings_server::nowcoder_contest_list_update(long long int contestId) {
        // 锁定比赛用户排名和题目统计数据的互斥锁
        std::lock(nowcoder_contest_map_mtx[contestId], nowcoder_contest_set_mtx[contestId],
                  Time_of_sealing_Submit_Map_mtx[contestId]);
        std::lock_guard<std::mutex> lock1(nowcoder_contest_map_mtx[contestId], std::adopt_lock);
        std::lock_guard<std::mutex> lock2(nowcoder_contest_set_mtx[contestId], std::adopt_lock);
        std::lock_guard<std::mutex> lock3(Time_of_sealing_Submit_Map_mtx[contestId], std::adopt_lock);
        try {
            // 获取比赛模板信息
            Contest_Info_Template contest_info_template; {
                std::lock_guard<std::mutex> contestInfoLock(Contest_Info_map_mtx);
                // 检查比赛是否存在
                if (Contest_Info_map.find(contestId) == Contest_Info_map.end()) {
                    // 如果找不到比赛信息，解锁并返回错误
                    return -1;
                }

                contest_info_template = Contest_Info_map[contestId];
            }

            {
                std::lock_guard<std::mutex> Memorize_the_assessment_records_Supplementary_orderLock(
                    Memorize_the_assessment_records_Supplementary_order_mtx[contestId]);
                // 检查比赛是否结束且无补充记录
                if (Memorize_the_assessment_records_Supplementary_order[contestId].size() == 0 && contest_info_template.
                    contestEndTime <= time(nullptr) * 1000) {
                    return 2; // 比赛已结束且无补充记录
                }
            }
            int max_submissionId;
            // 获取并更新榜单内以及更新进去的最大提交测评记录ID
            {
                std::lock_guard<std::mutex> maxIdLock(nowcoder_contest_list_max_submissionId_mtx);
                // 初始化最大提交ID为0（如果尚未设置）
                if (nowcoder_contest_list_max_submissionId.find(contestId) == nowcoder_contest_list_max_submissionId.
                    end()) {
                    nowcoder_contest_list_max_submissionId[contestId] = 0;
                }
                max_submissionId = nowcoder_contest_list_max_submissionId[contestId];
            }


            // 处理主评估记录
            for (auto &x: Memorize_the_assessment_records[contestId]) {
                // 如果当前记录ID等于最大提交ID，则停止处理
                if (x.first == max_submissionId) break;
                // 跳过比赛结束后的提交记录
                if (x.second.get_submit_time() >= contest_info_template.contestEndTime) continue;
                // 更新排名数据（模式0：主评估记录）
                update_ac_nowcoder_ranking_data(contestId, contest_info_template, x.second, 0);
            }

            // 处理补充评估记录
            vector<long long int> budanwancheng;
            for (auto &x: Memorize_the_assessment_records_Supplementary_order[contestId]) {
                // 如果记录需要处理
                if (x.second) {
                    budanwancheng.push_back(x.first);
                    // 确保记录存在再访问，并跳过比赛结束后的提交
                    if (Memorize_the_assessment_records[contestId].find(x.first) != Memorize_the_assessment_records[
                            contestId].end() &&
                        Memorize_the_assessment_records[contestId][x.first].get_submit_time() >= contest_info_template.
                        contestEndTime) {
                        continue;
                    }
                    // 确保记录存在再访问
                    if (Memorize_the_assessment_records[contestId].find(x.first) != Memorize_the_assessment_records[
                            contestId].end()) {
                        // 更新排名数据（模式1：补充评估记录）
                        update_ac_nowcoder_ranking_data(contestId, contest_info_template,
                                                        Memorize_the_assessment_records[contestId][x.first], 1);
                    }
                }
            }

            {
                std::lock_guard<std::mutex> Memorize_the_assessment_records_Supplementary_orderLock(
                    Memorize_the_assessment_records_Supplementary_order_mtx[contestId]);
                // 清理已处理的补充记录
                for (auto x: budanwancheng) {
                    Memorize_the_assessment_records_Supplementary_order[contestId].erase(x);
                }
            }

            // 解封榜单，处理封存记录
            // 检查解封条件：封存状态开启、无补充记录、有封存记录、到达解封时间
            if (contest_info_template.sealing_Status_code && Memorize_the_assessment_records_Supplementary_order[
                    contestId].
                size() == 0 && Time_of_sealing_Submit_Map[
                    contestId].size() > 0 && contest_info_template.Time_of_release <= time(nullptr) * 1000) {
                for (auto &x: Time_of_sealing_Submit_Map[contestId]) {
                    // 确保记录存在再访问，并跳过比赛结束后的提交
                    if (Memorize_the_assessment_records[contestId].find(x.first) != Memorize_the_assessment_records[
                            contestId].end() &&
                        Memorize_the_assessment_records[contestId][x.first].get_submit_time() >=
                        contest_info_template.
                        contestEndTime)
                        continue;

                    // 确保记录存在再访问
                    if (Memorize_the_assessment_records[contestId].find(x.first) != Memorize_the_assessment_records[
                            contestId].end()) {
                        // 更新排名数据（模式2：解封记录）
                        update_ac_nowcoder_ranking_data(contestId, contest_info_template,
                                                        Memorize_the_assessment_records[contestId][x.first], 2);
                    }
                }
                // 清空封存记录
                Time_of_sealing_Submit_Map[contestId].clear();
            }


            // 更新排行榜向量
            std::lock_guard<std::mutex> vectorLock(nowcoder_contest_vector_mtx[contestId]);
            int pm = 0;
            ac_nowcoder_Ranking_data ac_nowcoder_Ranking_data1;
            // 清空现有排行榜向量
            nowcoder_contest_vector[contestId].clear();
            // 遍历排序后的集合，更新排名并填充向量
            for (auto &item: nowcoder_contest_set[contestId]) {
                // 如果当前项与前一项不同（AC数或罚时不同），则排名递增
                if (!(item.acceptedCount == ac_nowcoder_Ranking_data1.acceptedCount && item.penalty_Time ==
                      ac_nowcoder_Ranking_data1.penalty_Time)) {
                    pm++;
                }
                // 设置排名
                item.ranking = pm;
                // 添加到排行榜向量
                nowcoder_contest_vector[contestId].push_back(item);
            }

            return 1; // 处理成功
        } catch (const std::exception &e) {
            // 记录异常信息（如果可能）
            return -1; // 发生异常
        } catch (...) {
            // 捕获所有其他异常
            return -1; // 发生未知异常
        }
    }
}
