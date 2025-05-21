//
// Created by fsptd on 25-5-20.
//

#ifndef AC_NOWCODER_RANKING_DATA_H
#define AC_NOWCODER_RANKING_DATA_H
#include "map"
#include "string"

namespace ac_nowcoder_rankings_server {
    // 题目状态结构体，记录题目相关信息
    struct Title_status {
        int Number_of_errors;  // 错误提交次数
        int status;            // 题目状态(0:未完成,1:已完成)
        int achh,acmm,acss;    // 完成时间(时,分,秒)
    };

    // 用户排名数据类
    class ac_nowcoder_Ranking_data {
    public:
        int userid;            // 用户ID
        int username;          // 用户名
        int ranking;           // 当前排名
        int acceptedCount;     // 已通过题目数量
        int penalty_Time;      // 罚时(单位:分钟)
        std::map<std::string,Title_status>Title_status_map; // 题目状态映射表
    };

    // 排名比较器结构体
    struct ac_nowcoder_Ranking_data_cmp {
        // 重载比较运算符
        bool operator()(const ac_nowcoder_Ranking_data& p1, const ac_nowcoder_Ranking_data& p2) const {
            // 优先比较通过题目数量
            if (p1.acceptedCount == p2.acceptedCount) {
                // 题目数量相同则比较罚时
                if (p1.penalty_Time==p2.penalty_Time) {
                    // 罚时相同则比较用户名
                    if (p1.username==p2.username)
                        return p1.userid<p2.userid;  // 用户名相同比较用户ID
                    return p1.username<p2.username;
                }
                return p1.penalty_Time < p2.penalty_Time;
            }
            return p1.acceptedCount > p2.acceptedCount;
        }
    };
}


#endif //AC_NOWCODER_RANKING_DATA_H
