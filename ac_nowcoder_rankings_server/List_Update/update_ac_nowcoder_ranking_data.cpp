//
// Created by fsptd on 25-6-26.
//
#include "../ac_nowcoder_rankings_server/ac_nowcoder_rankings_server.h"

namespace ac_nowcoder_rankings_server {
    int ac_nowcoder_rankings_server::update_ac_nowcoder_ranking_data(long long int contestId,
                                                                     Contest_Info_Template contest_info_template,
                                                                     Evaluation_Data_Template evaluation_data_template,
                                                                     bool Supplementary_order) {
        // 如果当前记录的提交ID等于最大提交ID，则跳出循环

        // 如果当前用户在当前比赛中不存在，则初始化用户数据
        if (nowcoder_contest_map[contestId].find(evaluation_data_template.get_user_id()) == nowcoder_contest_map[
                contestId].
            end()) {
            ac_nowcoder_Ranking_data a1;
            a1.userid = evaluation_data_template.get_user_id();
            a1.username = evaluation_data_template.get_user_name();
            // 初始化用户的所有题目状态
            for (int i = 0; i < contest_info_template.Title_number; i++) {
                string s1;
                s1.push_back('A' + i);
                a1.Title_status_map[s1].Title_Name = s1;
            }
            nowcoder_contest_map[contestId][a1.userid] = a1;
        }
        nowcoder_contest_map_mtx[contestId].lock();
        ac_nowcoder_Ranking_data ac_nowcoder_ranking_data1 = nowcoder_contest_map[contestId][evaluation_data_template.
            get_user_id()];
        nowcoder_contest_map_mtx[contestId].unlock();
        nowcoder_contest_set[contestId].erase(ac_nowcoder_ranking_data1);

        ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].Submission_record[
            evaluation_data_template.
            get_submission_id()] = evaluation_data_template;

        contest_info_template.problemData_map[evaluation_data_template.get_index()].submitCount++;

        // 锁定未记录评估状态映射的互斥锁，确保线程安全
        Unrecorded_Assessment_Status_Map_mtx.lock();
        // 如果当前记录的状态信是不会产生影响的CE和系统错误，则跳过该记录,但是需要记录本次提交的信息
        if (Unrecorded_Assessment_Status_Map.find(evaluation_data_template.get_status_message())
            != Unrecorded_Assessment_Status_Map.end()) {
            Unrecorded_Assessment_Status_Map_mtx.unlock();

            nowcoder_contest_map[contestId][ac_nowcoder_ranking_data1.userid] = ac_nowcoder_ranking_data1;
            // 将更新后的用户数据插入比赛集合
            nowcoder_contest_set[contestId].insert(ac_nowcoder_ranking_data1);
            return 1;
        }
        // 解锁未记录评估状态映射的互斥锁
        Unrecorded_Assessment_Status_Map_mtx.unlock();
        if (evaluation_data_template.get_status_message() == "正在判题") {
            if (ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].Submission_status ==
                0) {
                ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].Submission_status = 1;
            }
            ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].Submitted_not_completed_num
                    ++;
        }
        // 如果当前记录的状态信息为"答案正确"，则更新用户排名数据
        else if (evaluation_data_template.get_status_message() == "答案正确") {
            // 如果当前题目已经被标记为正确，则跳过该记录
            if (nowcoder_contest_map[contestId][evaluation_data_template.get_user_id()].Title_status_map[
                    evaluation_data_template.get_index()].
                status) {
            } else {
                // 标记当前题目为正确
                ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].status = 1;
                // 计算当前题目的AC时间(毫秒为单位)
                ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].AC_time =
                        evaluation_data_template.get_submit_time() - contest_info_template.contestBeginTime;
                // 计算当前题目的罚时: 每错误一次增加20分钟(1200000毫秒)罚时
                ac_nowcoder_ranking_data1.penalty_Time += 1200000 * ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].Number_of_errors;
                ac_nowcoder_ranking_data1.penalty_Time += ac_nowcoder_ranking_data1.Title_status_map[
                    evaluation_data_template.
                    get_index()].AC_time;
                // 增加用户的AC题目数量
                ac_nowcoder_ranking_data1.acceptedCount++;
                ac_nowcoder_ranking_data1.penalty_Time += ac_nowcoder_ranking_data1.Title_status_map[
                    evaluation_data_template.
                    get_index()].AC_time;
                // 如果当前题目的提交状态为0，则更新为1，标识本题已产生提交
                if (ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].Submission_status
                    == 0) {
                    ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].Submission_status =
                            1;
                }
                // 增加题目的AC次数
                contest_info_template.problemData_map[evaluation_data_template.get_index()].acceptedCount++;
                // 如果当前题目的状态为0，则更新为1，并标记为首杀
                if (contest_info_template.problemData_map[evaluation_data_template.get_index()].status == 0) {
                    contest_info_template.problemData_map[evaluation_data_template.get_index()].status = 1;
                    ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].first_Blood = true;
                }
            }
        } else {
            // 非AC状态的处理逻辑暂未实现
            if (ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].Submission_status ==
                0) {
                ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].Submission_status = 1;
            }
            ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].Number_of_errors++;
        }
        if (Supplementary_order) {
            ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].Submitted_not_completed_num
                    --;
        }
        nowcoder_contest_map[contestId][ac_nowcoder_ranking_data1.userid] = ac_nowcoder_ranking_data1;
        // 将更新后的用户数据插入比赛集合
        nowcoder_contest_set[contestId].insert(ac_nowcoder_ranking_data1);
        return 1;
    }
}
