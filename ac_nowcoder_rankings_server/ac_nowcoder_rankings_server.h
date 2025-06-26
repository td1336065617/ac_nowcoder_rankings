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

    /**
     * @brief 牛客比赛排名服务器类，继承自HTTP服务器框架
     *
     * 该类负责处理牛客平台比赛相关的排名逻辑，包括比赛数据的获取、排名更新、评估数据处理等
     */
    class ac_nowcoder_rankings_server : public HTTP_Server_Framework {
    public:
        /**
         * @brief 构造函数
         *
         * 初始化服务器时：
         * 1. 设置不记录的评估状态类型（编译错误和内部错误）
         * 2. 初始化线程池（最大100线程，核心5线程，任务队列容量10）
         * 3. 添加榜单更新分配中心任务到线程池
         */
        ac_nowcoder_rankings_server() {
            Unrecorded_Assessment_Status_Map["编译错误"] = 1;
            Unrecorded_Assessment_Status_Map["内部错误"] = 1;
            ac_nowcoder_rankings_server_thread_pool.set_Thread_Pool(100, 5, 10);
            Thread_Pool::Task task1([this]() {
                this->List_Update_Distribution_Center();
            });
            ac_nowcoder_rankings_server_thread_pool.addtanks(task1);
        }

        /**
         * @brief 析构函数
         *
         * 设置服务器关闭标志，通知所有线程停止工作
         */
        ~ac_nowcoder_rankings_server() {
            Shutdown_bj = 1;
        }

    private:
        // 互斥锁组，用于保护各类共享资源的线程安全访问
        mutex Memorize_the_assessment_records_mtx_mtx;  // 评估记录主锁的互斥锁
        map<long long int, mutex> Memorize_the_assessment_records_mtx;  // 按比赛ID细分的评估记录锁
        mutex ac_nowcoder_Ranking_data_cmp_mtx;  // 排名数据比较锁
        mutex nowcoder_contest_list_mtx;  // 比赛列表锁
        mutex nowcoder_contest_list_max_submissionId_mtx;  // 最大提交ID锁
        mutex Memorize_the_assessment_records_max_submissionId_mtx;  // 评估记录最大提交ID锁
        mutex Listen_to_the_competition_queue_mtx;  // 监听比赛队列锁
        mutex Stop_monitoring_the_competition_mtx;  // 停止监听比赛锁
        mutex Contest_Info_map_mtx;  // 比赛信息映射锁
        map<long long int, mutex> nowcoder_contest_set_mtx;  // 按比赛ID细分的比赛集合锁
        mutex nowcoder_contest_set_mtx_mtx;  // 比赛集合锁的互斥锁
        mutex Unrecorded_Assessment_Status_Map_mtx;  // 未记录评估状态映射锁
        mutex Memorize_the_assessment_records_Supplementary_order_mtx_mtx;  // 补充订单主锁的互斥锁
        map<long long int, mutex> Memorize_the_assessment_records_Supplementary_order_mtx;  // 按比赛ID细分的补充订单锁
        map<long long int, mutex> nowcoder_contest_map_mtx;  // 按比赛ID细分的比赛映射锁
        mutex nowcoder_contest_map_mtx_mtx;  // 比赛映射锁的互斥锁

        /**
         * @brief 任务分配中心
         * @param httpRequestData HTTP请求数据
         * @param httpResponseData HTTP响应数据（输出参数）
         * @return 处理结果状态码
         */
        int Task_distribution_center(HTTP_request_data httpRequestData, HTTP_response_data &httpResponseData);

        /**
         * @brief 获取指定比赛的评估数据
         * @param contestId 比赛ID
         * @param cookie 用户认证cookie
         * @return 处理结果状态码
         */
        int Get_Evaluation_Data(long long int contestId, string cookie);

        /**
         * @brief 获取单次请求的评估数据（支持分页）
         * @param contestId 比赛ID
         * @param cookie 用户认证cookie
         * @param page 当前页码
         * @param max_pageSize 最大页大小
         * @return 评估数据模板向量
         */
        vector<Evaluation_Data_Template> Get_Evaluation_Data_Single_request(
            long long int contestId, string cookie, int page, int max_pageSize);

        // 数据存储结构
        map<long long int, Contest_Info_Template> Contest_Info_map;  // 比赛ID到比赛信息的映射
        map<long long int, map<long long int, Evaluation_Data_Template, Evaluation_Data_cmp> >
            Memorize_the_assessment_records;  // 评估记录缓存（比赛ID->提交ID->评估数据）
        map<long long int, map<long long int, int, Evaluation_Data_cmp> >
            Memorize_the_assessment_records_Supplementary_order;  // 正在评估中的记录（比赛ID->提交ID->状态）
        map<long long int, long long int> Memorize_the_assessment_records_max_submissionId;  // 每个比赛的最大提交ID
        map<long long int, map<long long int, ac_nowcoder_Ranking_data> > nowcoder_contest_map;  // 比赛排名数据（比赛ID->用户ID->排名数据）
        map<long long int, set<ac_nowcoder_Ranking_data> > nowcoder_contest_set;  // 比赛排序集合（比赛ID->排序后的排名数据）
        map<long long int, long long int> nowcoder_contest_list_max_submissionId;  // 比赛榜单最大提交ID
        queue<Listen_to_the_competition_Template> Listen_to_the_competition_queue;  // 需要监听的比赛队列
        map<long long int, long long int> Stop_monitoring_the_competition;  // 需要停止监听的比赛映射
        map<string, int> Unrecorded_Assessment_Status_Map;  // 不记录的评估状态类型映射

        /**
         * @brief 榜单更新函数
         *
         * 负责实际更新比赛列表的排名数据
         */
        void List_update();

        /**
         * @brief 榜单更新分配中心
         *
         * 协调和分配榜单更新任务
         */
        void List_Update_Distribution_Center();

        /**
         * @brief 更新指定比赛的榜单
         * @param contestId 比赛ID
         * @return 处理结果状态码
         */
        int nowcoder_contest_list_update(long long int contestId);

        /**
         * @brief 更新牛客排名数据
         * @param contestId 比赛ID
         * @param contest_info_template 比赛信息模板
         * @param evaluation_data_template 评估数据模板
         * @param Supplementary_order 是否为补充订单
         * @return 处理结果状态码
         */
        int update_ac_nowcoder_ranking_data(long long int contestId,Contest_Info_Template contest_info_template,
                                          Evaluation_Data_Template evaluation_data_template,bool Supplementary_order);

        Thread_Pool::Thread_Pool ac_nowcoder_rankings_server_thread_pool;  // 线程池实例
        int Shutdown_bj = 0;  // 服务器关闭标志

        /**
         * @brief 获取比赛榜单
         * @param contest 比赛ID字符串
         * @return 榜单的字符串表示
         */
        string get_nowcoder_contest_list(string contest);

        /**
         * @brief 添加比赛信息
         * @param contestId 比赛ID
         * @param cookie 用户认证cookie
         * @return 处理结果状态码
         */
        int add_Contest_Info(long long int contestId, string cookie);
    };
} // ac_nowcoder_rankings_server
// 下一个版本计划实现Evaluation_Data_Template的记忆化互通
#endif //AC_NOWCODER_RANKINGS_SERVER_H
