//
// Created by fsptd on 25-5-20.
//
#include <string>
#include <map>
#include "../Evaluation Data Template/Evaluation_Data_Template.h"
#include "../ac_nowcoder_rankings_server.h"
#include "../tools_class/time_class.h"
#include"nlohmann/json.hpp"
/*
*namespace ac_nowcoder_rankings_server {
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
        int sealing_Status_code;//本次比赛是否存在封榜
        long long int Time_of_sealing;//封榜时间
        long long int Time_of_release;//解榜时间
        std::string rankType; // 排名类型
        std::map<std::string , problemData>problemData_map; // 题目数据映射表，键为题目ID，值为题目数据
    };
} // ac_nowcoder_rankings_server

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


 *
 */
namespace ac_nowcoder_rankings_server {
    string ac_nowcoder_rankings_server::get_nowcoder_contest_list(long long int contestId) {
        string ac_nowcoder_rankings_server_str;
        nlohmann::json ac_nowcoder_rankings_server_json;
        ac_nowcoder_rankings_server_json["contestId"] = contestId;
        Contest_Info_map_mtx.lock();
        Contest_Info_Template contest_info_template = Contest_Info_map[contestId];
        ac_nowcoder_rankings_server_json["contestname"] = contest_info_template.contestname;
        ac_nowcoder_rankings_server_json["contestId"] = contest_info_template.contestId;
        ac_nowcoder_rankings_server_json["Title_number"] = contest_info_template.Title_number;
        ac_nowcoder_rankings_server_json["contestEndTime"] = contest_info_template.contestEndTime;
        ac_nowcoder_rankings_server_json["contestBeginTime"] = contest_info_template.contestBeginTime;
        ac_nowcoder_rankings_server_json["sealing_Status_code"] = contest_info_template.sealing_Status_code;
        ac_nowcoder_rankings_server_json["Time_of_sealing"] = contest_info_template.Time_of_sealing;
        ac_nowcoder_rankings_server_json["Time_of_release"] = contest_info_template.Time_of_release;
        ac_nowcoder_rankings_server_json["rankType"] = contest_info_template.rankType;
        for (auto &item: contest_info_template.problemData_map) {
            ac_nowcoder_rankings_server_json["problemData_map"][item.first]["name"] = item.second.name;
            ac_nowcoder_rankings_server_json["problemData_map"][item.first]["status"] = item.second.status;
            ac_nowcoder_rankings_server_json["problemData_map"][item.first]["acceptedCount"] = item.second.
                    acceptedCount;
            ac_nowcoder_rankings_server_json["problemData_map"][item.first]["submitCount"] = item.second.submitCount;
        }

        nowcoder_contest_set_mtx[contestId].lock();
        ac_nowcoder_Ranking_data ac_nowcoder_Ranking_data1;
        int pm = 0;
        for (auto &item: nowcoder_contest_set[contestId]) {
            if (item.acceptedCount == ac_nowcoder_Ranking_data1.acceptedCount && item.penalty_Time ==
                ac_nowcoder_Ranking_data1.penalty_Time) {
            } else {
                pm++;
            }
            item.ranking = pm;
            ac_nowcoder_rankings_server_json["nowcoder_contest_set"]["ranking_data"][pm]["userid"] = item.userid;
            ac_nowcoder_rankings_server_json["nowcoder_contest_set"]["ranking_data"][pm]["username"] = item.username;
            ac_nowcoder_rankings_server_json["nowcoder_contest_set"]["ranking_data"][pm]["ranking"] = item.ranking;
            ac_nowcoder_rankings_server_json["nowcoder_contest_set"]["ranking_data"][pm]["acceptedCount"] = item.
                    acceptedCount;
            ac_nowcoder_rankings_server_json["nowcoder_contest_set"]["ranking_data"][pm]["penalty_Time"] = item.
                    penalty_Time;
            ac_nowcoder_rankings_server_json["nowcoder_contest_set"]["ranking_data"][pm]["ac_Finally_time"] = item.
                    ac_Finally_time;
            for (auto &item1: item.Title_status_map) {
                ac_nowcoder_rankings_server_json["nowcoder_contest_set"]["ranking_data"][pm]["ac_Finally_time"][
                    "Title_status_map"]["Title_Name"] = item1.second.Title_Name;
                ac_nowcoder_rankings_server_json["nowcoder_contest_set"]["ranking_data"][pm]["ac_Finally_time"][
                    "Title_status_map"]["status"] = item1.second.status;
                ac_nowcoder_rankings_server_json["nowcoder_contest_set"]["ranking_data"][pm]["ac_Finally_time"][
                    "Title_status_map"]["AC_time"] = item1.second.AC_time;
                ac_nowcoder_rankings_server_json["nowcoder_contest_set"]["ranking_data"][pm]["ac_Finally_time"][
                    "Title_status_map"]["first_Blood"] = item1.second.first_Blood;
                ac_nowcoder_rankings_server_json["nowcoder_contest_set"]["ranking_data"][pm]["ac_Finally_time"][
                    "Title_status_map"]["Number_of_errors"] = item1.second.Number_of_errors;
                ac_nowcoder_rankings_server_json["nowcoder_contest_set"]["ranking_data"][pm]["ac_Finally_time"][
                    "Title_status_map"]["Submission_status"] = item1.second.Submission_status;
                ac_nowcoder_rankings_server_json["nowcoder_contest_set"]["ranking_data"][pm]["ac_Finally_time"][
                    "Title_status_map"]["Submitted_not_completed_num"] = item1.second.Submitted_not_completed_num;
            }
        }
        ac_nowcoder_rankings_server_str = ac_nowcoder_rankings_server_json.dump();
        nowcoder_contest_set_mtx[contestId].unlock();
        Contest_Info_map_mtx.unlock();

        return ac_nowcoder_rankings_server_str;
    }
}
