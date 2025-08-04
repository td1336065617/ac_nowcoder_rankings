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
        nowcoder_contest_map_mtx[contestId].lock();
        nowcoder_contest_set_mtx[contestId].lock();
        Time_of_sealing_Submit_Map_mtx[contestId].lock();
        // 获取比赛模板信息
        Contest_Info_map_mtx.lock();
        Contest_Info_Template contest_info_template = Contest_Info_map[contestId];
        Contest_Info_map_mtx.unlock();

        // 检查比赛是否结束且无补充记录
        if (Memorize_the_assessment_records_Supplementary_order[contestId].size() == 0 && contest_info_template.
            contestEndTime <= time(nullptr) * 1000) {
            nowcoder_contest_set_mtx[contestId].unlock();
            nowcoder_contest_map_mtx[contestId].unlock();
            return 2; // 比赛已结束且无补充记录
        }

        // 获取并更新最大提交ID
        nowcoder_contest_list_max_submissionId_mtx.lock();
        if (nowcoder_contest_list_max_submissionId.find(contestId) == nowcoder_contest_list_max_submissionId.end()) {
            nowcoder_contest_list_max_submissionId[contestId] = 0;
        }
        int max_submissionId = nowcoder_contest_list_max_submissionId[contestId];
        nowcoder_contest_list_max_submissionId_mtx.unlock();

        // 处理主评估记录
        for (auto &x: Memorize_the_assessment_records[contestId]) {
            if (x.first == max_submissionId) break;
            if (x.second.get_submit_time() >= contest_info_template.contestEndTime) continue;
            update_ac_nowcoder_ranking_data(contestId, contest_info_template, x.second, 0);
        }

        // 处理补充评估记录
        vector<long long int> budanwancheng;
        for (auto &x: Memorize_the_assessment_records_Supplementary_order[contestId]) {
            if (x.first) {
                if (Memorize_the_assessment_records[contestId][x.first].get_submit_time() >= contest_info_template.
                    contestEndTime) {
                    budanwancheng.push_back(x.first);
                    continue;
                }
                update_ac_nowcoder_ranking_data(contestId, contest_info_template,
                                                Memorize_the_assessment_records[contestId][x.first], 1);
            }
        }
        // 清理已处理的补充记录
        for (auto x: budanwancheng) {
            Memorize_the_assessment_records_Supplementary_order[contestId].erase(x);
        }
        //解封榜单，处理封存记录
        if (contest_info_template.sealing_Status_code && Memorize_the_assessment_records_Supplementary_order[contestId].
            size() == 0 && Time_of_sealing_Submit_Map[
                contestId].size() > 0 && contest_info_template.Time_of_release <= time(nullptr) * 1000) {
            for (auto &x: Time_of_sealing_Submit_Map[contestId]) {
                if (Memorize_the_assessment_records[contestId][x.first].get_submit_time() >= contest_info_template.
                    contestEndTime)
                    continue;

                update_ac_nowcoder_ranking_data(contestId, contest_info_template,
                                                Memorize_the_assessment_records[contestId][x.first], 2);
            }
            Time_of_sealing_Submit_Map[contestId].clear();
        }
        nowcoder_contest_vector_mtx[contestId].lock();
        int pm = 0,i=1;
        ac_nowcoder_Ranking_data ac_nowcoder_Ranking_data1;
        for (auto &item: nowcoder_contest_set[contestId]) {
            if (!(item.acceptedCount == ac_nowcoder_Ranking_data1.acceptedCount && item.penalty_Time ==
                ac_nowcoder_Ranking_data1.penalty_Time)) {
                pm++;
            }
            item.ranking = pm;
            nowcoder_contest_vector[contestId].push_back(item);
        }
        nowcoder_contest_vector_mtx[contestId].unlock();

        // 解锁互斥锁
        nowcoder_contest_set_mtx[contestId].unlock();
        nowcoder_contest_map_mtx[contestId].unlock();
        Time_of_sealing_Submit_Map_mtx[contestId].unlock();
        return 1; // 处理成功
    }
}
