//
// Created by fsptd on 25-5-21.
//
#include <string>
#include <map>
#include "Evaluation Data Template/Evaluation_Data_Template.h"
#include "ac_nowcoder_rankings_server.h"
#include "tools_class/time_class.h"
#include"nlohmann/json.hpp"

namespace ac_nowcoder_rankings_server {
    using namespace nlohmann;
    int ac_nowcoder_rankings_server::add_Contest_Info(long long int contestId, string cookie) {
        try {
            time_class time_class1; // 用于生成时间戳的时间工具类实例
            std::map<long long int, Evaluation_Data_Template> Evaluation_Data_Map; // 评估数据缓存(未实际使用)

            // 构造第一个URL，用于获取实时排名数据
            string url1 = "https://ac.nowcoder.com/acm-heavy/acm/contest/real-time-rank-data?token=&id="
                        + std::to_string(contestId) + "&page=1&limit=0&searchUserName=&_="
                        + std::to_string(time_class1.get_time_ms());

            // 构造第二个URL，用于获取相同比赛列表数据
            string url2 = "https://ac.nowcoder.com/acm/contest/rank/same-contest-list?token=&contestId="
                + std::to_string(contestId) + "&_="
                + std::to_string(time_class1.get_time_ms());

            // 发送HTTP请求并获取响应
            string str1 = epollRequest.get_url(url1, cookie);
            string str2 = epollRequest.get_url(url2, cookie);

            // 检查响应是否为空
            if (str1.empty() || str2.empty()) {
                std::cerr << "Empty response from URL request" << std::endl;
                return -1;
            }

            // 解析JSON响应
            json j1, j2;
            try {
                j1 = json::parse(str1);
                j2 = json::parse(str2);
            } catch (const json::parse_error& e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
                return -1;
            }

            // 检查JSON响应中是否包含"data"字段
            if (!j1.contains("data") || !j2.contains("data")) {
                std::cerr << "Missing 'data' field in JSON response" << std::endl;
                return -1;
            }

            // 提取基本信息和其他比赛列表
            json basicInfo = j1["data"]["basicInfo"];
            json sameContests = j2["data"]["sameContests"];

            // 检查JSON数据中是否包含必要的字段
            if (!basicInfo.contains("contestId") || !basicInfo.contains("pageCount") ||
                !basicInfo.contains("contestEndTime") || !basicInfo.contains("contestBeginTime") ||
                !basicInfo.contains("rankType") || sameContests.empty() || !sameContests[0].contains("name")) {
                std::cerr << "Missing required fields in JSON data" << std::endl;
                return -1;
            }

            // 填充比赛信息模板
            Contest_Info_Template contest_info_template;
            contest_info_template.contestId = basicInfo["contestId"].get<long long int>();
            if (j1["data"].contains("problemData")) {
                contest_info_template.Title_number = j1["data"]["problemData"].size();
            } else {
                contest_info_template.Title_number = 0; // 如果problemData字段不存在，默认设置为0
            }
            contest_info_template.contestEndTime = basicInfo["contestEndTime"].get<long long int>();
            contest_info_template.contestBeginTime = basicInfo["contestBeginTime"].get<long long int>();
            contest_info_template.rankType = basicInfo["rankType"].get<std::string>();
            contest_info_template.contestname = sameContests[0]["name"];

            // 将比赛信息插入到全局映射中
            Contest_Info_map_mtx.lock();
            Contest_Info_map.insert(std::make_pair(contestId, contest_info_template));
            Contest_Info_map_mtx.unlock();

            // 创建监听比赛模板并插入到队列中
            Listen_to_the_competition_Template listen_to_the_competition_template;
            listen_to_the_competition_template.contest_id = contestId;
            listen_to_the_competition_template.cookie = cookie;
            Listen_to_the_competition_queue_mtx.lock();
            Listen_to_the_competition_queue.push(listen_to_the_competition_template);
            Listen_to_the_competition_queue_mtx.unlock();
            Memorize_the_assessment_records_mtx_mtx.lock();
            Memorize_the_assessment_records_mtx[contestId];
            Memorize_the_assessment_records_mtx_mtx.unlock();
            Memorize_the_assessment_records_Supplementary_order_mtx_mtx.lock();

        } catch (const std::exception& e) {
            // 捕获并处理标准异常
            std::cerr << "Exception caught: " << e.what() << std::endl;
            return -1; // 返回错误码
        } catch (...) {
            // 捕获所有其他异常
            std::cerr << "Unknown exception caught" << std::endl;
            return -1; // 返回错误码
        }
        return 1; // 返回成功码
    }

}