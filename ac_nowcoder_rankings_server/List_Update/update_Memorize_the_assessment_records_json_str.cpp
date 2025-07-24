//
// Created by fsptd on 25-7-24.
//
#include "../ac_nowcoder_rankings_server.h"
#include "nlohmann/json.hpp"

namespace ac_nowcoder_rankings_server {
    /**
     * @brief 更新牛客竞赛集的JSON字符串表示
     *
     * 该函数根据给定的竞赛ID，从Contest_Info_map中获取竞赛信息，
     * 并结合nowcoder_contest_set中的排名数据，构建一个完整的JSON对象。
     * 最终将该JSON对象序列化为字符串，并存储到nowcoder_contest_set_json_str中。
     *
     * @param contestId 竞赛ID
     * @return int 返回1表示更新成功
     */
    int ac_nowcoder_rankings_server::update_nowcoder_contest_set_json_str(long long int contestId) {
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

        nowcoder_contest_set_json_str_mtx[contestId].lock();
        nowcoder_contest_set_json_str[contestId] = ac_nowcoder_rankings_server_str;
        nowcoder_contest_set_json_str_mtx[contestId].unlock();
        return 1;
    }

}