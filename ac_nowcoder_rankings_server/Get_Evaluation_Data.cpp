//
// Created by fsptd on 25-5-15.
//

#include <string>
#include <map>
#include "Evaluation Data Template/Evaluation_Data_Template.h"
#include "ac_nowcoder_rankings_server.h"
#include "tools_class/time_class.h"

namespace ac_nowcoder_rankings_server {
    std::map<std::string, Evaluation_Data_Template> ac_nowcoder_rankings_server::Get_Evaluation_Data(
        std::string contest, std::string cookie){
        // Placeholder implementation
        time_class time_class1;
        std::map<std::string, Evaluation_Data_Template> Evaluation_Data_Map;
        string url = "	https://ac.nowcoder.com/acm-heavy/acm/contest/status-list?token=&id="
                     + contest + "&page=1&pageSize=50&searchUserName=&_="
                     + std::to_string(time_class1.get_time_ms());

        string str = epollRequest.get_url(url, cookie);

        return Evaluation_Data_Map;
    }
} // ac_nowcoder_rankings_server
