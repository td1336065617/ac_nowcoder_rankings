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
        int Submission_status;  // 提交状态，表示题目的提交情况
        int Number_of_errors;  // 错误提交次数，记录用户提交错误的次数
        int status;            // 题目状态(0:未完成,1:已完成)，表示题目是否已完成
        long long int AC_time;    // 完成时间，记录用户完成题目的时间
        bool first_Blood;      // 是否为首杀，表示用户是否是第一个完成该题目的人
    };

    // 用户排名数据类，用于存储用户的排名信息
    class ac_nowcoder_Ranking_data {
    public:
        long long int  userid;            // 用户ID，唯一标识用户
        std::string username;          // 用户名，用户的昵称
        long long int  ranking;           // 当前排名，用户在排行榜中的位置
        long long int  acceptedCount;     // 已通过题目数量，用户已完成的题目数量
        long long int penalty_Time;      // 罚时(单位:毫秒)，用户的总罚时
        std::map<std::string,Title_status>Title_status_map; // 题目状态映射表，记录用户每个题目的状态
    };

    // 排名比较器结构体，用于比较两个用户的排名
    struct ac_nowcoder_Ranking_data_cmp {
        // 重载比较运算符，用于比较两个用户的排名
        bool operator()(const ac_nowcoder_Ranking_data& p1, const ac_nowcoder_Ranking_data& p2) const {
            // 优先比较通过题目数量，通过题目数量多的用户排名靠前
            if (p1.acceptedCount == p2.acceptedCount) {
                // 题目数量相同则比较罚时，罚时少的用户排名靠前
                if (p1.penalty_Time==p2.penalty_Time) {
                    // 罚时相同则比较用户名，按字典序排列
                    if (p1.username==p2.username)
                        return p1.userid<p2.userid;  // 用户名相同比较用户ID，ID小的用户排名靠前
                    return p1.username<p2.username;
                }
                return p1.penalty_Time < p2.penalty_Time;
            }
            return p1.acceptedCount > p2.acceptedCount;
        }
    };
}


#endif //AC_NOWCODER_RANKING_DATA_H
