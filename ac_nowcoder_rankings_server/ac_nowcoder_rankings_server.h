//
// Created by fsptd on 25-5-14.
//
#include "../include/HTTP_Server_Framework/HTTP_Server_Framework.h"
#include "Evaluation Data Template/Evaluation_Data_Template.h"
#include "ac_nowcoder_Ranking_data/ac_nowcoder_Ranking_data.h"
#include "set"
#include "../include/Thread_Pool/Thread_Pool.h"
#include "Listen_to_the_competition_Template/Listen_to_the_competition_Template.h"
#include "Contest_Info_Template/Contest_Info_Template.h"
#ifndef AC_NOWCODER_RANKINGS_SERVER_H
#define AC_NOWCODER_RANKINGS_SERVER_H

namespace ac_nowcoder_rankings_server {
    using namespace HTTP_Server_Framework;

    // ac_nowcoder_rankings_server类，继承自HTTP_Server_Framework，用于处理牛客比赛排名相关逻辑
    class ac_nowcoder_rankings_server : public HTTP_Server_Framework {
    public:
        // 构造函数，初始化时向线程池添加任务
        ac_nowcoder_rankings_server() {
            Unrecorded_Assessment_Status_Map["编译错误"]=1;
            Unrecorded_Assessment_Status_Map["内部错误"]=1;
            ac_nowcoder_rankings_server_thread_pool.set_Thread_Pool(100,5,10);
            Thread_Pool::Task task1([this]() {
                 this->List_Update_Distribution_Center();
             });
            ac_nowcoder_rankings_server_thread_pool.addtanks(task1);
        }

        // 析构函数，设置关闭标志
        ~ac_nowcoder_rankings_server() {
            Shutdown_bj = 1;
        }

    private:
        // 互斥锁，用于保护评估记录和排名数据的线程安全
        mutex Memorize_the_assessment_records_mtx_mtx;
        map<long long int,mutex>Memorize_the_assessment_records_mtx;
        mutex ac_nowcoder_Ranking_data_cmp_mtx;
        mutex nowcoder_contest_list_mtx;
        mutex nowcoder_contest_list_max_submissionId_mtx;
        mutex Memorize_the_assessment_records_max_submissionId_mtx;
        mutex Listen_to_the_competition_queue_mtx;
        mutex Stop_monitoring_the_competition_mtx;
        mutex Contest_Info_map_mtx;
        mutex nowcoder_contest_set_mtx;
        mutex Unrecorded_Assessment_Status_Map_mtx;
        mutex Memorize_the_assessment_records_Supplementary_order_mtx_mtx;
        map<long long int,mutex> Memorize_the_assessment_records_Supplementary_order_mtx;
        mutex nowcoder_contest_map_mtx;
        // 任务分配中心，处理HTTP请求并生成响应
        int Task_distribution_center(HTTP_request_data httpRequestData, HTTP_response_data &httpResponseData);

        // 获取指定比赛和cookie的评估数据
        int Get_Evaluation_Data(long long int contestId, string cookie);

        // 获取单次请求的评估数据，支持分页
        vector<Evaluation_Data_Template> Get_Evaluation_Data_Single_request(
            long long int contestId, string cookie, int page, int max_pageSize);

        map<long long int,Contest_Info_Template>Contest_Info_map;
        // 记忆化存储的评估记录，用于缓存评估数据以提高性能
        map<long long int, map<long long int, Evaluation_Data_Template, Evaluation_Data_cmp>> Memorize_the_assessment_records;
        //如果存在正在测评的记录，等测评完成，更新入排行榜
        map<long long int, map<long long int,int, Evaluation_Data_cmp>>Memorize_the_assessment_records_Supplementary_order;
        // 记录每个比赛的最大提交ID，用于跟踪最新评估数据
        map<long long int, long long int> Memorize_the_assessment_records_max_submissionId;

        // 存储牛客比赛榜单，键为比赛ID，值为参赛者的排名数据
        map<long long int, map<long long int, ac_nowcoder_Ranking_data>> nowcoder_contest_map;
        //通过set自定义排序规则，排序排名数据。
        map<long long int, set<ac_nowcoder_Ranking_data>>nowcoder_contest_set;
        // 记录每个比赛更新入排行榜内的最大测记录提交ID，用于跟踪最新提交数据，键为比赛ID，值为最大提交ID
        map<long long int, long long int> nowcoder_contest_list_max_submissionId;
        // 监听比赛队列，用于处理需要监听的比赛
        queue<Listen_to_the_competition_Template> Listen_to_the_competition_queue;
        // 停止监听比赛，用于处理需要停止监听的比赛
        map<long long int, long long int> Stop_monitoring_the_competition;
        //
        map<string,int>Unrecorded_Assessment_Status_Map;
        // 榜单更新函数，负责实际更新比赛列表
        void List_update();
        // 榜单更新分配中心，负责协调列表更新任务
        void List_Update_Distribution_Center();
        int nowcoder_contest_list_update(long long int contestId);
        // 线程池，用于并发处理任务，提高系统吞吐量
        Thread_Pool::Thread_Pool ac_nowcoder_rankings_server_thread_pool;
        int Shutdown_bj = 0;
        // 获取指定比赛的牛客比赛榜单，返回榜单的字符串表示
        string get_nowcoder_contest_list(string contest);
        int add_Contest_Info(long long int contestId, string cookie);
    };
} // ac_nowcoder_rankings_server
// 下一个版本计划实现Evaluation_Data_Template的记忆化互通
#endif //AC_NOWCODER_RANKINGS_SERVER_H
