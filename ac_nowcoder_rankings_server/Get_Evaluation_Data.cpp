//
// Created by fsptd on 25-5-15.
//

#include <string>
#include <map>
#include "Evaluation Data Template/Evaluation_Data_Template.h"
#include "ac_nowcoder_rankings_server.h"
namespace ac_nowcoder_rankings_server {
    std::map<std::string, Evaluation_Data_Template> ac_nowcoder_rankings_server::Get_Evaluation_Data(std::string contest,std::string cookie); {
        // Placeholder implementation
        std::map<std::string, Evaluation_Data_Template>Evaluation_Data_Map;
        string url="	https://ac.nowcoder.com/acm-heavy/acm/contest/status-list?token=&id="+contest+"&page=1&pageSize=20&searchUserName=&_=1747291566469";
        url+=contest;
        string str=epollRequest.get_url(url);

        return Evaluation_Data_Map;
    }
} // ac_nowcoder_rankings_server
