//
// Created by fsptd on 25-5-15.
//

#include <string>
#include <map>
#include "Evaluation Data Template/Evaluation_Data_Template.h"
#include "ac_nowcoder_rankings_server.h"
#include "tools_class/time_class.h"
#include"nlohmann/json.hpp"

namespace ac_nowcoder_rankings_server {
    using namespace nlohmann;

    /**
     * @brief 获取比赛评估数据
     * @param contestId 比赛ID
     * @param cookie 用户认证cookie
     * @return int 函数执行状态码(1表示成功)
     *
     * 详细功能流程:
     * 1. 构造带时间戳的请求URL
     * 2. 发送HTTP GET请求获取比赛提交状态
     * 3. 解析返回的JSON数据:
     *    - 基础响应信息(code/msg)
     *    - 比赛结束状态(isContestFinished)
     *    - 分页信息(页数、大小、当前页等)
     * 4. 分页获取所有评估数据:
     *    - 获取每页数据
     *    - 处理每一条提交记录:
     *      * 更新缓存记录
     *      * 处理"正在判题"状态的提交
     *      * 跟踪最小/最大提交ID
     *      * 检查提前终止条件
     * 5. 更新全局最大提交ID缓存
     * 6. 输出当前比赛记录数
     */
    int ac_nowcoder_rankings_server::Get_Evaluation_Data(
        long long int contestId, std::string cookie) {
        try {
            time_class time_class1; // 时间工具类实例，用于生成请求时间戳
            std::map<long long int, Evaluation_Data_Template> Evaluation_Data_Map; // 评估数据缓存(当前未使用)

            /* 构造带时间戳的请求URL */
            string url = "https://ac.nowcoder.com/acm-heavy/acm/contest/status-list?token=&id="
                         + std::to_string(contestId) + "&page=1&pageSize=50&searchUserName=&_="
                         + std::to_string(time_class1.get_time_ms());

            /* 执行HTTP GET请求并解析基础JSON响应 */
            string str = epollRequest.get_url(url, cookie);
            json j1, j1_data;
            j1 = json::parse(str);
            string msg = j1["msg"].get<string>(); // API返回消息
            int code = j1["code"].get<int>(); // HTTP状态码
            j1_data = j1["data"]; // 实际数据节点
            bool data_isContestFinished = j1_data["isContestFinished"].get<bool>(); // 比赛是否结束标志

            /* 解析分页和基础信息 */
            json j1_data_basicInfo = j1_data["basicInfo"]; // 基础信息节点
            int basicUid = j1_data_basicInfo["basicUid"].get<int>(); // 用户UID
            contestId = j1_data_basicInfo["contestId"].get<long long int>(); // 比赛ID
            int pageCount = j1_data_basicInfo["pageCount"].get<int>(); // 总页数
            int pageSize = j1_data_basicInfo["pageSize"].get<int>(); // 每页大小
            int statusCount = j1_data_basicInfo["statusCount"].get<int>(); // 总记录数
            string searchUserName = j1_data_basicInfo["searchUserName"].get<string>(); // 搜索用户名
            int pageCurrent = j1_data_basicInfo["pageCurrent"].get<int>(); // 当前页码

            /* 初始化提交ID追踪变量 */
            long long int All_min_submission_id = LLONG_MAX, min_submission_id = LLONG_MAX;
            long long int max_submission_id = 0; // 当前处理的最大提交ID
            long long int Unfinished_assessment = 1e17; // 正在判题的提交最小ID

            /* 分页获取所有评估数据 */
            for (int i = 0; i < pageCount + 20; i++) {
                int max_submission_bj = 0; // 提前终止标志

                /* 获取当前页的评估数据 */
                vector<Evaluation_Data_Template> evaluation_data_templates_list =
                        Get_Evaluation_Data_Single_request(contestId, cookie, i, 50);

                /* 处理当前页数据 */
                for (auto &evaluation_data_templates: evaluation_data_templates_list) {
                    /* 更新缓存记录 */
                    {
                        std::lock_guard<std::mutex> lock(Memorize_the_assessment_records_mtx[contestId]);
                        Memorize_the_assessment_records[contestId][evaluation_data_templates.get_submission_id()] =
                                evaluation_data_templates;
                    }

                    /* 处理"正在判题"状态的提交 */
                    if (evaluation_data_templates.get_status_message() == "正在判题") {
                        Unfinished_assessment = min(Unfinished_assessment, evaluation_data_templates.get_submission_id());
                        {
                            std::lock_guard<std::mutex> lock(Memorize_the_assessment_records_Supplementary_order_mtx[contestId]);
                            Memorize_the_assessment_records_Supplementary_order[contestId][evaluation_data_templates.get_submission_id()] = 0;
                        }
                    }

                    /* 更新补充订单状态 */
                    {
                        std::lock_guard<std::mutex> lock(Memorize_the_assessment_records_Supplementary_order_mtx[contestId]);
                        if (Memorize_the_assessment_records_Supplementary_order[contestId].find(evaluation_data_templates.get_submission_id())
                            != Memorize_the_assessment_records_Supplementary_order[contestId].end()) {
                            if (evaluation_data_templates.get_status_message() != "正在判题") {
                                Memorize_the_assessment_records_Supplementary_order[contestId][evaluation_data_templates.get_submission_id()] = 1;
                            }
                        }
                    }

                    /* 更新最小提交ID */
                    min_submission_id = min(min_submission_id, evaluation_data_templates.get_submission_id());

                    /* 检查是否需要提前终止 */
                    {
                        std::lock_guard<std::mutex> lock(Memorize_the_assessment_records_max_submissionId_mtx);
                        if (min_submission_id < Memorize_the_assessment_records_max_submissionId[contestId]) {
                            max_submission_bj = 1;
                            break;
                        }
                    }

                    /* 更新最大提交ID */
                    max_submission_id = max(max_submission_id, evaluation_data_templates.get_submission_id());
                }

                /* 终止条件处理 */
                if (i >= pageCount && All_min_submission_id == min_submission_id || max_submission_bj) break;
                All_min_submission_id = min_submission_id; // 更新全局最小ID
            }

            /* 更新最大提交ID缓存 */
            {
                std::lock_guard<std::mutex> lock(Memorize_the_assessment_records_max_submissionId_mtx);
                if (Unfinished_assessment == 1e17) {
                    Memorize_the_assessment_records_max_submissionId[contestId] = max_submission_id;
                } else {
                    Memorize_the_assessment_records_max_submissionId[contestId] = Unfinished_assessment;
                }
            }

            /* 输出当前比赛记录数 */
            cout << Memorize_the_assessment_records[contestId].size() << endl;
            return 1; // 执行成功
        } catch (const std::exception& e) {
            // 记录异常日志
            std::cerr << "Exception in Get_Evaluation_Data: " << e.what() << std::endl;
            return -1; // 返回错误码
        } catch (...) {
            // 捕获所有其他异常
            std::cerr << "Unknown exception in Get_Evaluation_Data" << std::endl;
            return -2; // 返回未知错误码
        }
    }
} // ac_nowcoder_rankings_server
