//
// Created by fsptd on 25-5-21.
//
#include <string>
#ifndef CONTEST_INFO_TEMPLATE_H
#define CONTEST_INFO_TEMPLATE_H

namespace ac_nowcoder_rankings_server {
    // 比赛信息模板类
    class Contest_Info_Template {
    public:
        std::string contestname; // 比赛名称
        long long int contestId; // 比赛ID
        int pageCount;//题目数
        long long int contestEndTime; // 比赛结束时间
        long long int contestBeginTime; // 比赛开始时间
        std::string rankType; // 排名类型

    };
} // ac_nowcoder_rankings_server

#endif //CONTEST_INFO_TEMPLATE_H
