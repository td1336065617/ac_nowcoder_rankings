//
// Created by fsptd on 25-7-24.
//
#include "ac_nowcoder_rankings_server.h"
#include "nlohmann/json.hpp"

namespace ac_nowcoder_rankings_server {
    int ac_nowcoder_rankings_server::Update_Memorize_the_assessment_records_str(long long int contestId) {
        string mtarstr;
        try {
            string ac_nowcoder_contest_info_str; // 存储最终返回的JSON字符串
            nlohmann::json ac_nowcoder_contest_info_json; // 用于构建JSON对象

            // 检查contestId是否有效，如果小于等于0则抛出无效参数异常
            if (contestId <= 0) {
                throw std::invalid_argument("Invalid contest ID");
            }

            // 加锁，保证线程安全，如果找不到对应的contestId则抛出运行时异常
            if (!Memorize_the_assessment_records_mtx.count(contestId)) {
                throw std::runtime_error("Contest not found");
            }
            Memorize_the_assessment_records_mtx[contestId].lock();

            try {
                // 设置比赛基本信息
                ac_nowcoder_contest_info_json["contestId"] = contestId; // 比赛ID
                ac_nowcoder_contest_info_json["contest_info_size"] = Memorize_the_assessment_records[contestId].size(); // 记录数量

                // 遍历比赛中的所有评估记录，为每条记录设置详细信息
                for (auto &evaluation_Data: Memorize_the_assessment_records[contestId]) {
                    // 使用submissionId作为唯一标识符存储每条记录的详细信息
                    ac_nowcoder_contest_info_json["contest_infos"][evaluation_Data.second.get_submission_id()]["submissionId"] = evaluation_Data.second.get_submission_id(); // 提交ID
                    ac_nowcoder_contest_info_json["contest_infos"][evaluation_Data.second.get_submission_id()]["memory"] = evaluation_Data.second.get_memory(); // 内存使用情况（单位：KB）
                    ac_nowcoder_contest_info_json["contest_infos"][evaluation_Data.second.get_submission_id()]["length"] = evaluation_Data.second.get_length(); // 代码长度（单位：字符）
                    ac_nowcoder_contest_info_json["contest_infos"][evaluation_Data.second.get_submission_id()]["index"] = evaluation_Data.second.get_index(); // 题目索引
                    ac_nowcoder_contest_info_json["contest_infos"][evaluation_Data.second.get_submission_id()]["language"] = evaluation_Data.second.get_language(); // 编程语言
                    ac_nowcoder_contest_info_json["contest_infos"][evaluation_Data.second.get_submission_id()]["userName"] = evaluation_Data.second.get_user_name(); // 用户名
                    ac_nowcoder_contest_info_json["contest_infos"][evaluation_Data.second.get_submission_id()]["userId"] = evaluation_Data.second.get_user_id(); // 用户ID
                    ac_nowcoder_contest_info_json["contest_infos"][evaluation_Data.second.get_submission_id()]["colorLevel"] = evaluation_Data.second.get_color_level(); // 颜色等级（表示题目难度）
                    ac_nowcoder_contest_info_json["contest_infos"][evaluation_Data.second.get_submission_id()]["statusMessage"] = evaluation_Data.second.get_status_message(); // 状态信息（如AC、WA等）
                    ac_nowcoder_contest_info_json["contest_infos"][evaluation_Data.second.get_submission_id()]["submitTime"] = evaluation_Data.second.get_submit_time(); // 提交时间（UNIX时间戳）
                    ac_nowcoder_contest_info_json["contest_infos"][evaluation_Data.second.get_submission_id()]["time"] = evaluation_Data.second.get_time(); // 运行时间（单位：ms）
                    ac_nowcoder_contest_info_json["contest_infos"][evaluation_Data.second.get_submission_id()]["problemId"] = evaluation_Data.second.get_problem_id(); // 题目ID
                }
            } catch (const std::exception &e) {
                // 如果在处理过程中出现异常，先解锁再抛出异常
                Memorize_the_assessment_records_mtx[contestId].unlock();
                throw std::runtime_error(std::string("Error processing contest data: ") + e.what());
            }

            // 解锁，释放资源
            Memorize_the_assessment_records_mtx[contestId].unlock();

            // 将JSON对象转为字符串并输出到控制台
            ac_nowcoder_contest_info_str = ac_nowcoder_contest_info_json.dump();
            cout << ac_nowcoder_contest_info_str << endl;

            // 更新全局的JSON字符串缓存，加锁保证线程安全
            Memorize_the_assessment_records_json_str_mtx[contestId].lock();
            Memorize_the_assessment_records_json_str[contestId] = ac_nowcoder_contest_info_str;
            Memorize_the_assessment_records_json_str_mtx[contestId].unlock();

            return 1; // 成功返回1
        } catch (const std::exception &e) {
            return -1; // 出现异常返回-1
        }
    }
}
