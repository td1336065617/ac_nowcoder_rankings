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
        time_class time_class1; // 用于生成时间戳的时间工具类实例
        std::map<long long int, Evaluation_Data_Template> Evaluation_Data_Map; // 评估数据缓存(未实际使用)
        string url1 = "https://ac.nowcoder.com/acm-heavy/acm/contest/real-time-rank-data?token=&id="
                    + std::to_string(contestId) + "&page=1&limit=0&searchUserName=&_="
                    + std::to_string(time_class1.get_time_ms());
        string url2 = "	https://ac.nowcoder.com/acm/contest/rank/same-contest-list?token=&contestId="
            + std::to_string(contestId) + "&_="
            + std::to_string(time_class1.get_time_ms());
        string str1 = epollRequest.get_url(url1, cookie);
        string str2 = epollRequest.get_url(url2, cookie);
        json j1, j2;
        j1 = json::parse(str1);
        j2 = json::parse(str2);
        json basicInfo=j1["data"]["basicInfo"];
        json sameContests=j2["data"]["sameContests"];
        Contest_Info_Template contest_info_template;
        contest_info_template.contestId=basicInfo["contestId"].get<long long int>();
        contest_info_template.pageCount=basicInfo["pageCount"].get<int>();
        contest_info_template.contestEndTime=basicInfo["contestBeginTime"].get<long long int>();
        contest_info_template.contestBeginTime=basicInfo["contestBeginTime"].get<long long int>();
        contest_info_template.rankType=basicInfo["contestBeginTime"].get<std::string>();
        contest_info_template.contestname=sameContests[0]["name"];
        Contest_Info_map_mtx.lock();
        Contest_Info_map.insert(std::make_pair(contestId, contest_info_template));
        Contest_Info_map_mtx.unlock();
        return 1;
    }

}