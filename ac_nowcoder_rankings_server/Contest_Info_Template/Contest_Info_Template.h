//
// Created by fsptd on 25-5-21.
//
#ifndef CONTEST_INFO_TEMPLATE_H
#define CONTEST_INFO_TEMPLATE_H
#include <string>
#include <map>
namespace ac_nowcoder_rankings_server {
    // 比赛信息模板类
    struct problemData {
        std::string name; // 题目名称
        int status; // 题目状态
        int acceptedCount; // 通过次数
        int submitCount; // 提交次数
    };
    class Contest_Info_Template {
    public:
        std::string contestname; // 比赛名称
        long long int contestId; // 比赛ID
        int Title_number; // 题目数量
        long long int contestEndTime; // 比赛结束时间
        long long int contestBeginTime; // 比赛开始时间
        std::string rankType; // 排名类型
        std::map<std::string , problemData>problemData_map; // 题目数据映射表，键为题目ID，值为题目数据
    };
} // ac_nowcoder_rankings_server

#endif //CONTEST_INFO_TEMPLATE_H
