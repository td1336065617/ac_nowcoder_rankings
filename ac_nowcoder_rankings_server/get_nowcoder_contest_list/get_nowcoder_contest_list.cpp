//
// Created by fsptd on 25-5-20.
//
#include <string>
#include <map>
#include "../Evaluation Data Template/Evaluation_Data_Template.h"
#include "../ac_nowcoder_rankings_server.h"
#include "../tools_class/time_class.h"
#include"nlohmann/json.hpp"
namespace ac_nowcoder_rankings_server {
    string ac_nowcoder_rankings_server::get_nowcoder_contest_list(long long int contestId) {
        string ac_nowcoder_rankings_server_str;
        nlohmann::json ac_nowcoder_rankings_server_json;
        ac_nowcoder_rankings_server_json["contestId"] = contestId;
        
        return ac_nowcoder_rankings_server_str;
    }

}