//
// Created by fsptd on 25-5-21.
//
#include "../ac_nowcoder_rankings_server/ac_nowcoder_rankings_server.h"
namespace ac_nowcoder_rankings_server {
    int ac_nowcoder_rankings_server::nowcoder_contest_list_update(long long int contestId) {
        nowcoder_contest_list_max_submissionId_mtx.lock();
        if (nowcoder_contest_list_max_submissionId.find(contestId)==nowcoder_contest_list_max_submissionId.end()) {
            nowcoder_contest_list_max_submissionId[contestId]=0;
        }
        int max_submissionId=nowcoder_contest_list_max_submissionId[contestId];
        nowcoder_contest_list_max_submissionId_mtx.unlock();
        Contest_Info_map_mtx.lock();
        Contest_Info_Template contest_info_template= Contest_Info_map[contestId];
        Contest_Info_map_mtx.unlock();
        for (auto &x:  Memorize_the_assessment_records[contestId]) {
            if (x.first==max_submissionId) {
                break;
            }
            if (x.second.get_submit_time()>=contest_info_template.contestEndTime) {
                continue;
            }
            if (x.second.get_status_message()=="AC") {
                if (nowcoder_contest_list[contestId].find(x.second.get_user_id())==nowcoder_contest_list[contestId].end()) {


                }else {
                    nowcoder_contest_list_mtx.lock();
                if (nowcoder_contest_list[contestId][x.second.get_user_id()].Title_status_map[x.second.get_index()].status) {
                    continue;
                    nowcoder_contest_list_mtx.unlock();
                }
                ac_nowcoder_Ranking_data ac_nowcoder_ranking_data1=nowcoder_contest_list[contestId][x.second.get_user_id()];
                ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].status=1;
                    ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].AC_time=x.second.get_submit_time()-contest_info_template.contestBeginTime;
                    ac_nowcoder_ranking_data1.penalty_Time+=1200000*ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].Number_of_errors;
                    ac_nowcoder_ranking_data1.penalty_Time+=ac_nowcoder_ranking_data1.Title_status_map[x.second.get_index()].AC_time;
                    ac_nowcoder_ranking_data1.acceptedCount++;
                    ac_nowcoder_ranking_data1.penalty_Time+=x.second.get_submit_time()-contest_info_template.contestBeginTime;
                    nowcoder_contest_list_mtx.unlock();
                }

            }
        }
        return 1;
    }
}