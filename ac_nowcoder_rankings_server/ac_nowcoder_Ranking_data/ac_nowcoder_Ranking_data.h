//
// Created by fsptd on 25-5-20.
//

#ifndef AC_NOWCODER_RANKING_DATA_H
#define AC_NOWCODER_RANKING_DATA_H
#include "map"
#include "string"
#include "../Evaluation Data Template/Evaluation_Data_Template.h"
namespace ac_nowcoder_rankings_server {
    // 题目状态结构体，用于记录题目的相关状态信息
    struct Title_status {
        std::string Title_Name;
        int Submission_status=0;  // 提交状态：0表示未提交，1表示已提交
        int Number_of_errors=0;   // 错误提交次数：记录用户提交该题目时的错误次数
        int status=0;             // 题目状态：0表示未完成，1表示已完成
        long long int AC_time=0;  // 完成时间：记录用户完成该题目的时间，单位为毫秒
        bool first_Blood=false;       // 是否为首杀：true表示用户是第一个完成该题目的人，false表示不是
        int Submitted_not_completed_num;//已经提交但未完成测评的测评条数。
        std::map<long long int,Evaluation_Data_Template>Submission_record;

    };

    // 用户排名数据类，用于存储用户在排行榜中的相关信息
    class ac_nowcoder_Ranking_data {
    public:
        long long int  userid=-1;            // 用户ID：唯一标识用户
        std::string username="";          // 用户名：用户的昵称
        long long int  ranking=-1;           // 当前排名：用户在排行榜中的位置
        long long int  acceptedCount=0;     // 已通过题目数量：用户已完成的题目数量
        long long int penalty_Time=0;      // 罚时：用户的总罚时，单位为毫秒
        long long int ac_Finally_time=0;   // 最后通过时间：用户最后通过题目的时间，单位为毫秒
        std::map<std::string,Title_status>Title_status_map; // 题目状态映射表：记录用户每个题目的状态信息

        // 重载小于运算符，用于比较两个用户的排名
        bool operator < (const ac_nowcoder_Ranking_data& p1) const {
            // 优先比较通过题目数量，通过题目数量多的用户排名靠前
            if (acceptedCount == p1.acceptedCount) {
                // 如果通过题目数量相同，则比较罚时，罚时少的用户排名靠前
                if (penalty_Time == p1.penalty_Time) {
                    // 如果罚时也相同，则比较最后通过时间，时间早的用户排名靠前
                    if (ac_Finally_time == p1.ac_Finally_time) {
                        // 如果最后通过时间也相同，则比较用户名，按字典序排列
                        if (username == p1.username)
                            return userid < p1.userid;  // 如果用户名相同，则比较用户ID，ID小的用户排名靠前
                        return username < p1.username;
                    }
                    return ac_Finally_time < p1.ac_Finally_time;
                }
                return penalty_Time < p1.penalty_Time;
            }
            return acceptedCount > p1.acceptedCount;
        }
    };
}


#endif //AC_NOWCODER_RANKING_DATA_H
