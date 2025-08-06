//
// Created by fsptd on 25-6-26.
//
#include "../ac_nowcoder_rankings_server/ac_nowcoder_rankings_server.h"

namespace ac_nowcoder_rankings_server {
    /**
     * @brief 更新牛客比赛排名数据
     * @param contestId 比赛ID，唯一标识一场比赛
     * @param contest_info_template 比赛信息模板，包含比赛基础信息如题目数量、封榜时间等
     * @param evaluation_data_template 评测数据模板，包含用户提交的评测结果信息
     * @param Supplementary_order 是否为补充订单，用于标记是否为后期补充的评测数据
     * @return 操作结果，1表示成功，其他值表示失败
     *
     * 功能说明：
     * 1. 检查用户是否已存在，不存在则初始化新用户数据
     * 2. 线程安全地获取用户当前排名数据
     * 3. 处理封榜期间的提交数据
     * 4. 记录提交信息并更新题目提交计数
     * 5. 过滤不需要记录的评测状态(如编译错误等)
     * 6. 处理不同评测状态：
     *    - 正在判题：更新提交状态
     *    - 答案正确：计算AC时间、罚时，更新AC计数，处理首杀
     *    - 其他状态：增加错误计数
     * 7. 特殊处理补充订单的情况
     * 8. 更新用户数据并重新插入比赛集合
     */
    int ac_nowcoder_rankings_server::update_ac_nowcoder_ranking_data(long long int contestId,
                                                                     Contest_Info_Template contest_info_template,
                                                                     Evaluation_Data_Template evaluation_data_template,
                                                                     bool Supplementary_order) {
        try {
            // 检查用户是否已存在于当前比赛中
            if (nowcoder_contest_map[contestId].find(evaluation_data_template.get_user_id()) == nowcoder_contest_map[
                    contestId].
                end()) {
                // 初始化新用户数据
                ac_nowcoder_Ranking_data a1;
                a1.userid = evaluation_data_template.get_user_id(); // 设置用户ID
                a1.username = evaluation_data_template.get_user_name(); // 设置用户名
                // 初始化用户的所有题目状态(A-Z)
                for (int i = 0; i < contest_info_template.Title_number; i++) {
                    string s1;
                    s1.push_back('A' + i);
                    a1.Title_status_map[s1].Title_Name = s1; // 初始化每个题目的状态信息
                }
                nowcoder_contest_map[contestId][a1.userid] = a1; // 将新用户加入比赛映射中
            }

            // 线程安全操作：获取用户数据
            ac_nowcoder_Ranking_data ac_nowcoder_ranking_data1 = nowcoder_contest_map[contestId][
                evaluation_data_template.
                get_user_id()];
            // 从比赛集合中移除旧数据
            nowcoder_contest_set[contestId].erase(ac_nowcoder_ranking_data1);

            // 判断是否处于封榜时间段且不是补充订单类型2
            if (contest_info_template.sealing_Status_code && contest_info_template.Time_of_sealing <=
                evaluation_data_template.get_submit_time() && Supplementary_order != 2) {
                evaluation_data_template.set_status_message("正在判题"); // 设置为判题中状态
                evaluation_data_template.set_memory(0); // 内存设为0
                evaluation_data_template.set_time(0); // 时间设为0
                Time_of_sealing_Submit_Map[contestId][evaluation_data_template.get_submission_id()] = 1; // 标记该提交在封榜期间
            }

            // 记录当前提交信息
            ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].Submission_record[
                evaluation_data_template.
                get_submission_id()] = evaluation_data_template;

            // 增加题目提交计数
            contest_info_template.problemData_map[evaluation_data_template.get_index()].submitCount++;

            // 检查是否为不需要记录的评测状态(CE/系统错误等)
            Unrecorded_Assessment_Status_Map_mtx.lock();
            if (Unrecorded_Assessment_Status_Map.find(evaluation_data_template.get_status_message())
                != Unrecorded_Assessment_Status_Map.end()) {
                Unrecorded_Assessment_Status_Map_mtx.unlock();

                // 仅更新用户数据而不处理排名
                nowcoder_contest_map[contestId][ac_nowcoder_ranking_data1.userid] = ac_nowcoder_ranking_data1;
                nowcoder_contest_set[contestId].insert(ac_nowcoder_ranking_data1);
                return 1;
            }
            Unrecorded_Assessment_Status_Map_mtx.unlock();

            // 处理不同评测状态
            if (evaluation_data_template.get_status_message() == "正在判题") {
                // 更新提交状态
                if (ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].Submission_status
                    ==
                    0) {
                    ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].Submission_status =
                            1;
                }
                // 增加未完成测评的提交计数
                ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].
                        Submitted_not_completed_num
                        ++;
            }
            // 处理AC(答案正确)状态
            else if (evaluation_data_template.get_status_message() == "答案正确") {
                // 如果题目尚未AC
                if (!nowcoder_contest_map[contestId][evaluation_data_template.get_user_id()].Title_status_map[
                        evaluation_data_template.get_index()].
                    status) {
                    // 标记题目为AC
                    ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].status = 1;
                    // 计算AC时间(从比赛开始到提交的时间差)
                    ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].AC_time =
                            evaluation_data_template.get_submit_time() - contest_info_template.contestBeginTime;
                    // 计算罚时(每错误一次加20分钟)
                    ac_nowcoder_ranking_data1.penalty_Time += 1200000 * ac_nowcoder_ranking_data1.Title_status_map[
                        evaluation_data_template.get_index()].Number_of_errors;
                    ac_nowcoder_ranking_data1.penalty_Time += ac_nowcoder_ranking_data1.Title_status_map[
                        evaluation_data_template.
                        get_index()].AC_time;
                    // 更新用户AC计数
                    ac_nowcoder_ranking_data1.acceptedCount++;
                    // 标记题目已提交
                    if (ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].
                        Submission_status
                        == 0) {
                        ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].
                                Submission_status =
                                1;
                    }
                    // 更新题目AC计数
                    contest_info_template.problemData_map[evaluation_data_template.get_index()].acceptedCount++;
                    // 检查并标记首杀
                    if (contest_info_template.problemData_map[evaluation_data_template.get_index()].status == 0) {
                        contest_info_template.problemData_map[evaluation_data_template.get_index()].status = 1;
                        ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].first_Blood =
                                true;
                    }
                }
            } else {
                if (!nowcoder_contest_map[contestId][evaluation_data_template.get_user_id()].Title_status_map[
                        evaluation_data_template.get_index()].
                    status) {
                    // 处理其他评测状态(WA/TLE/RE等)
                    if (ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].
                        Submission_status ==
                        0) {
                        ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].
                                Submission_status = 1;
                    }
                    // 增加错误计数
                    ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].Number_of_errors++;
                }
            }

            // 减少未完成测评的提交计数（如果是补充订单）
            if (Supplementary_order) {
                ac_nowcoder_ranking_data1.Title_status_map[evaluation_data_template.get_index()].
                        Submitted_not_completed_num
                        --;
            }

            // 更新用户数据
            nowcoder_contest_map[contestId][ac_nowcoder_ranking_data1.userid] = ac_nowcoder_ranking_data1;
            // 将更新后的用户数据重新插入比赛集合
            nowcoder_contest_set[contestId].insert(ac_nowcoder_ranking_data1);
            return 1;
        } catch (const std::exception &e) {
            // 记录异常日志
            // 可以使用日志库记录异常信息
            // 例如: LOG_ERROR << "Exception in update_ac_nowcoder_ranking_data: " << e.what();
            return -1; // 返回错误码
        } catch (...) {
            // 捕获所有其他异常
            // 可以使用日志库记录未知异常
            // 例如: LOG_ERROR << "Unknown exception in update_ac_nowcoder_ranking_data";
            return -2; // 返回错误码
        }
    }
}
