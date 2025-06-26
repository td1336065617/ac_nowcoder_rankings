//
// Created by fsptd on 25-5-21.
//
#include "../ac_nowcoder_rankings_server/ac_nowcoder_rankings_server.h"

namespace ac_nowcoder_rankings_server {
    int ac_nowcoder_rankings_server::nowcoder_contest_list_update(long long int contestId) {
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

            // 锁定未记录评估状态映射的互斥锁，确保线程安全
            Unrecorded_Assessment_Status_Map_mtx.lock();
            // 如果当前记录的状态信是不会产生影响的CE和系统错误，则跳过该记录
            if (Unrecorded_Assessment_Status_Map.find(x.second.get_status_message())
                !=Unrecorded_Assessment_Status_Map.end()) {
                Unrecorded_Assessment_Status_Map_mtx.unlock();
                continue;
            }
            // 解锁未记录评估状态映射的互斥锁
            Unrecorded_Assessment_Status_Map_mtx.unlock();

            // 如果当前用户在当前比赛中不存在，则初始化用户数据
            if (nowcoder_contest_map[contestId].find(x.second.get_user_id()) == nowcoder_contest_map[contestId].
                   end()) {
                ac_nowcoder_Ranking_data a1;
                a1.userid=x.second.get_user_id();
                a1.username=x.second.get_user_name();
                // 初始化用户的所有题目状态
                for (int i=0;i<contest_info_template.Title_number;i++) {
                    string s1;s1.push_back('A'+i);
                    a1.Title_status_map[s1].Title_Name=s1;
                }
                nowcoder_contest_map[contestId][a1.userid]=a1;
            }

            // 如果当前记录的状态信息为“答案正确”，则更新用户排名数据
            if (x.second.get_status_message() == "答案正确") {
                    // 锁定比赛列表的互斥锁，确保线程安全
                    nowcoder_contest_list_mtx.lock();
                    // 如果当前题目已经被标记为正确，则跳过该记录
                    if (nowcoder_contest_map[contestId][x.second.get_user_id()].Title_status_map[x.second.get_index()].
                        status) {
                        continue;
                        nowcoder_contest_list_mtx.unlock();
                    }

                    // 获取当前用户的排名数据
                    ac_nowcoder_Ranking_data ac_nowcoder_ranking_data1 = nowcoder_contest_map[contestId][x.second.
                        get_user_id()];

                    // 从比赛集合中移除当前用户的数据
                    nowcoder_contest_set[contestId].erase(ac_nowcoder_ranking_data1);
                    // 标记当前题目为正确
                    ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].status = 1;
                    // 计算当前题目的AC时间
                    ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].AC_time =
                            x.second.get_submit_time() - contest_info_template.contestBeginTime;
                    // 计算当前题目的罚时
                    ac_nowcoder_ranking_data1.penalty_Time += 1200000 * ac_nowcoder_ranking_data1.Title_status_map[x.
                        second.get_index()].Number_of_errors;
                    ac_nowcoder_ranking_data1.penalty_Time += ac_nowcoder_ranking_data1.Title_status_map[x.second.
                        get_index()].AC_time;
                    // 增加用户的AC题目数量
                    ac_nowcoder_ranking_data1.acceptedCount++;
                    ac_nowcoder_ranking_data1.penalty_Time += x.second.get_submit_time() - contest_info_template.
                            contestBeginTime;
                    // 如果当前题目的提交状态为0，则更新为1，并增加题目的提交次数
                    if (ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].Submission_status==0) {
                        ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].Submission_status=1;
                        contest_info_template.problemData_map[x.second.get_index()].submitCount++;
                    }
                    // 增加题目的AC次数
                    contest_info_template.problemData_map[x.second.get_index()].acceptedCount++;
                    // 如果当前题目的状态为0，则更新为1，并标记为首杀
                    if (contest_info_template.problemData_map[x.second.get_index()].status==0) {
                    contest_info_template.problemData_map[x.second.get_index()].status = 1;
                        ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].first_Blood = true;
                    }
                    // 锁定比赛集合的互斥锁，确保线程安全
                    nowcoder_contest_set_mtx.lock();
                    // 从比赛集合中移除当前用户的数据
                    nowcoder_contest_set[contestId].erase(nowcoder_contest_map[contestId][ac_nowcoder_ranking_data1.userid]);
                    // 更新当前用户的数据
                    nowcoder_contest_map[contestId][ac_nowcoder_ranking_data1.userid]=ac_nowcoder_ranking_data1;
                    // 将更新后的用户数据插入比赛集合
                    nowcoder_contest_set[contestId].insert(ac_nowcoder_ranking_data1);
                    // 解锁比赛集合的互斥锁
                    nowcoder_contest_set_mtx.unlock();

                    // 解锁比赛列表的互斥锁
                    nowcoder_contest_list_mtx.unlock();
            }else {
                // 如果当前记录的状态信息不为“答案正确”，则不进行任何操作


            }
        }
        return 1;
    }
}
