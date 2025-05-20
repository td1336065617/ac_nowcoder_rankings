//
// Created by fsptd on 25-5-20.
//

#ifndef AC_NOWCODER_RANKING_DATA_H
#define AC_NOWCODER_RANKING_DATA_H
#include "map"
#include "string"

namespace ac_nowcoder_rankings_server {
    struct Title_status {
        int Number_of_errors;
        int status;
        int achh,acmm,acss;
    };
    class ac_nowcoder_Ranking_data {
    public:
        int userid;
        int username;
        int ranking;
        int acceptedCount;
        int penalty_Time;
        std::map<std::string,Title_status>Title_status_map;
    };
    struct ac_nowcoder_Ranking_data_cmp {
        bool operator()(const ac_nowcoder_Ranking_data& p1, const ac_nowcoder_Ranking_data& p2) const {
            if (p1.acceptedCount == p2.acceptedCount) {
                if (p1.penalty_Time==p2.penalty_Time) {
                    if (p1.username==p2.username)
                        return p1.userid<p2.userid;
                    return p1.username<p2.username;
                }
                return  p1.penalty_Time< p2.penalty_Time;
            }
            return p1.acceptedCount > p2.acceptedCount;
        }
    };
}


#endif //AC_NOWCODER_RANKING_DATA_H
