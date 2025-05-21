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

        return 1;
    }
}