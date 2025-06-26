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
            // 如果当前记录的提交ID等于最大提交ID，则跳出循环
            if (x.first == max_submissionId) {
                break;
            }
            // 如果当前记录的提交时间大于比赛结束时间，则跳过该记录
            if (x.second.get_submit_time() >= contest_info_template.contestEndTime) {
                continue;
            }
            // 如果当前用户在当前比赛中不存在，则初始化用户数据
            if (nowcoder_contest_map[contestId].find(x.second.get_user_id()) == nowcoder_contest_map[contestId].
                end()) {
                ac_nowcoder_Ranking_data a1;
                a1.userid = x.second.get_user_id();
                a1.username = x.second.get_user_name();
                // 初始化用户的所有题目状态
                for (int i = 0; i < contest_info_template.Title_number; i++) {
                    string s1;
                    s1.push_back('A' + i);
                    a1.Title_status_map[s1].Title_Name = s1;
                }
                nowcoder_contest_map[contestId][a1.userid] = a1;
            }
            nowcoder_contest_map_mtx[contestId].lock();
            ac_nowcoder_Ranking_data ac_nowcoder_ranking_data1 = nowcoder_contest_map[contestId][x.second.
               get_user_id()];
            nowcoder_contest_map_mtx[contestId].unlock();
            nowcoder_contest_set[contestId].erase(ac_nowcoder_ranking_data1);

            ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].Submission_record[x.second.
                      get_submission_id()] = x.second;

            contest_info_template.problemData_map[x.second.get_index()].submitCount++;

            // 锁定未记录评估状态映射的互斥锁，确保线程安全
            Unrecorded_Assessment_Status_Map_mtx.lock();
            // 如果当前记录的状态信是不会产生影响的CE和系统错误，则跳过该记录,但是需要记录本次提交的信息
            if (Unrecorded_Assessment_Status_Map.find(x.second.get_status_message())
                != Unrecorded_Assessment_Status_Map.end()) {
                Unrecorded_Assessment_Status_Map_mtx.unlock();
                continue;
                }
            // 解锁未记录评估状态映射的互斥锁
            Unrecorded_Assessment_Status_Map_mtx.unlock();
            if (x.second.get_status_message() == "正在判题") {
                // 非AC状态的处理逻辑暂未实现
                if (ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].Submission_status == 0) {
                    ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].Submission_status = 1;
                }
                ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].Submitted_not_completed_num++;
            }
            // 如果当前记录的状态信息为"答案正确"，则更新用户排名数据
            else if (x.second.get_status_message() == "答案正确") {
                // 如果当前题目已经被标记为正确，则跳过该记录
                if (nowcoder_contest_map[contestId][x.second.get_user_id()].Title_status_map[x.second.get_index()].
                    status) {
                    continue;
                }else {
                    // 标记当前题目为正确
                    ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].status = 1;
                    // 计算当前题目的AC时间(毫秒为单位)
                    ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].AC_time =
                            x.second.get_submit_time() - contest_info_template.contestBeginTime;
                    // 计算当前题目的罚时: 每错误一次增加20分钟(1200000毫秒)罚时
                    ac_nowcoder_ranking_data1.penalty_Time += 1200000 * ac_nowcoder_ranking_data1.Title_status_map[x.
                        second.get_index()].Number_of_errors;
                    ac_nowcoder_ranking_data1.penalty_Time += ac_nowcoder_ranking_data1.Title_status_map[x.second.
                        get_index()].AC_time;
                    // 增加用户的AC题目数量
                    ac_nowcoder_ranking_data1.acceptedCount++;
                    ac_nowcoder_ranking_data1.penalty_Time += ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].AC_time;
                    // 如果当前题目的提交状态为0，则更新为1，标识本题已产生提交
                    if (ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].Submission_status == 0) {
                        ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].Submission_status = 1;
                    }
                    // 增加题目的AC次数
                    contest_info_template.problemData_map[x.second.get_index()].acceptedCount++;
                    // 如果当前题目的状态为0，则更新为1，并标记为首杀
                    if (contest_info_template.problemData_map[x.second.get_index()].status == 0) {
                        contest_info_template.problemData_map[x.second.get_index()].status = 1;
                        ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].first_Blood = true;
                    }

                }

            } else {
                // 非AC状态的处理逻辑暂未实现
                if (ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].Submission_status == 0) {
                    ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].Submission_status = 1;
                }
                ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].Number_of_errors++;

            }
            nowcoder_contest_map[contestId][ac_nowcoder_ranking_data1.userid] = ac_nowcoder_ranking_data1;
            // 将更新后的用户数据插入比赛集合
            nowcoder_contest_set[contestId].insert(ac_nowcoder_ranking_data1);
        }
        nowcoder_contest_set_mtx[contestId].unlock();
        nowcoder_contest_map_mtx[contestId].unlock();
        return 1;
    }
}
