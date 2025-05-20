//
// Created by fsptd on 25-5-14.
//
#include "../include/HTTP_Server_Framework/HTTP_Server_Framework.h"
#include "Evaluation Data Template/Evaluation_Data_Template.h"
#include "set"
#ifndef AC_NOWCODER_RANKINGS_SERVER_H
#define AC_NOWCODER_RANKINGS_SERVER_H

namespace ac_nowcoder_rankings_server {
    using namespace HTTP_Server_Framework;
    class ac_nowcoder_rankings_server : public HTTP_Server_Framework {
    public:
    private:
        mutex Memorize_the_assessment_records_mtx;
        int Task_distribution_center(HTTP_request_data httpRequestData, HTTP_response_data &httpResponseData);
        //如果需要对HTTP请求的数据进行处理，继承后重写任务分配中心()
        map<long long int,Evaluation_Data_Template> Get_Evaluation_Data(string contest,string cookie);
        vector<Evaluation_Data_Template> Get_Evaluation_Data_Single_request(string contest,string cookie,int page,int max_pageSize);
        map<long long int,map<long long int,Evaluation_Data_Template>>Memorize_the_assessment_records;
        map<long long int,set<Evaluation_Data_Template>>nowcoder_contest_list;
        string get_nowcoder_contest_list(string contest);
    };
} // ac_nowcoder_rankings_server
//下一个版本做一下Evaluation_Data_Template的记忆化互通
#endif //AC_NOWCODER_RANKINGS_SERVER_H
