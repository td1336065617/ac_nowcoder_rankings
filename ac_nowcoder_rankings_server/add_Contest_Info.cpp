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

    /**
     * @brief 添加比赛信息到系统
     * @param contestId 比赛ID
     * @param cookie 用于认证的cookie
     * @param sealing_Status_code 封榜状态码
     * @param Time_of_sealing 封榜时间(毫秒时间戳)
     * @param Time_of_release 解榜时间(毫秒时间戳)
     * @return 1表示成功，-1表示失败
     */
    int ac_nowcoder_rankings_server::add_Contest_Info(long long int contestId, string cookie,int sealing_Status_code,long long int Time_of_sealing,long long int Time_of_release) {
        try {
            time_class time_class1; // 时间工具类实例，用于生成时间戳
            std::map<long long int, Evaluation_Data_Template> Evaluation_Data_Map; // 评估数据缓存(预留字段，暂未使用)

            // 构造获取实时排名数据的URL
            string url1 = "https://ac.nowcoder.com/acm-heavy/acm/contest/real-time-rank-data?token=&id="
                          + std::to_string(contestId) + "&page=1&limit=0&searchUserName=&_="
                          + std::to_string(time_class1.get_time_ms());

            // 构造获取相同比赛列表的URL
            string url2 = "https://ac.nowcoder.com/acm/contest/rank/same-contest-list?token=&contestId="
                          + std::to_string(contestId) + "&_="
                          + std::to_string(time_class1.get_time_ms());

            // 发送HTTP请求获取响应数据
            string str1 = epollRequest.get_url(url1, cookie);
            string str2 = epollRequest.get_url(url2, cookie);

            // 检查响应数据是否为空
            if (str1.empty() || str2.empty()) {
                std::cerr << "Empty response from URL request" << std::endl;
                return -1;
            }

            // 解析JSON格式的响应数据
            json j1, j2;
            try {
                j1 = json::parse(str1);
                j2 = json::parse(str2);
            } catch (const json::parse_error &e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
                return -1;
            }

            // 验证JSON数据中是否包含必需的data字段
            if (!j1.contains("data") || !j2.contains("data")) {
                std::cerr << "Missing 'data' field in JSON response" << std::endl;
                return -1;
            }

            // 从JSON数据中提取比赛基本信息和相同比赛列表
            json basicInfo = j1["data"]["basicInfo"];
            json sameContests = j2["data"]["sameContests"];

            // 验证JSON数据中是否包含所有必需的字段
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
                contest_info_template.Title_number = 0; // 默认题目数量为0
            }
            contest_info_template.contestEndTime = basicInfo["contestEndTime"].get<long long int>();
            contest_info_template.contestBeginTime = basicInfo["contestBeginTime"].get<long long int>();
            contest_info_template.rankType = basicInfo["rankType"].get<std::string>();
            contest_info_template.contestname = sameContests[0]["name"];
            contest_info_template.sealing_Status_code=sealing_Status_code;
            contest_info_template.Time_of_sealing=Time_of_sealing;
            contest_info_template.Time_of_release=Time_of_release;

            // 将比赛信息添加到全局映射中(线程安全)
            Contest_Info_map_mtx.lock();
            Contest_Info_map.insert(std::make_pair(contestId, contest_info_template));
            Contest_Info_map_mtx.unlock();

            // 创建监听比赛模板并添加到监听队列中(线程安全)
            Listen_to_the_competition_Template listen_to_the_competition_template;
            listen_to_the_competition_template.contest_id = contestId;
            listen_to_the_competition_template.cookie = cookie;
            Listen_to_the_competition_queue_mtx.lock();
            Listen_to_the_competition_queue.push(listen_to_the_competition_template);
            Listen_to_the_competition_queue_mtx.unlock();

            // 初始化比赛相关的互斥锁和数据结构(线程安全)
            Memorize_the_assessment_records_mtx_mtx.lock();
            Memorize_the_assessment_records_mtx[contestId];
            Memorize_the_assessment_records_mtx_mtx.unlock();

            Memorize_the_assessment_records_Supplementary_order_mtx_mtx.lock();
            Memorize_the_assessment_records_Supplementary_order_mtx[contestId];
            Memorize_the_assessment_records_Supplementary_order_mtx_mtx.unlock();

            Time_of_sealing_Submit_Map_mtx_mtx.lock();
            Time_of_sealing_Submit_Map_mtx[contestId];
            Time_of_sealing_Submit_Map_mtx_mtx.unlock();
        } catch (const std::exception &e) {
            // 处理标准异常
            std::cerr << "Exception caught: " << e.what() << std::endl;
            return -1;
        } catch (...) {
            // 处理未知异常
            std::cerr << "Unknown exception caught" << std::endl;
            return -1;
        }
        return 1; // 操作成功
    }
}
