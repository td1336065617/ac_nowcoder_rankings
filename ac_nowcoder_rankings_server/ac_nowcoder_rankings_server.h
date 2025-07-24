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
#include "../include/HTTP_Server_Framework/Time_data/Time_data.h"
#ifndef AC_NOWCODER_RANKINGS_SERVER_H
#define AC_NOWCODER_RANKINGS_SERVER_H

namespace ac_nowcoder_rankings_server {
    using namespace HTTP_Server_Framework;

    /**
     * @brief 牛客比赛排名服务器类，继承自HTTP服务器框架
     *
     * 该类负责处理牛客平台比赛相关的排名逻辑，包括但不限于：
     * - 比赛数据的获取与解析
     * - 实时排名更新与维护
     * - 评估数据处理与缓存
     * - 线程安全的资源管理
     * - 榜单生成与分发
     */
    class ac_nowcoder_rankings_server : public HTTP_Server_Framework {
    public:
        /**
         * @brief 构造函数
         *
         * 执行初始化操作：
         * 1. 配置不记录的评估状态类型（编译错误和内部错误）
         * 2. 初始化线程池（最大线程数100，核心线程数5，任务队列容量10）
         * 3. 启动榜单更新分配中心作为后台任务
         * 4. 确保线程安全的数据结构初始化
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
         * 执行清理操作：
         * 1. 设置服务器关闭标志
         * 2. 通知所有工作线程安全退出
         * 3. 确保资源有序释放
         */
        ~ac_nowcoder_rankings_server() {
            Shutdown_bj = 1;
        }

    private:
        // 线程同步机制
        mutex Memorize_the_assessment_records_mtx_mtx,nowcoder_contest_set_json_str_mtx_mtx,Memorize_the_assessment_records_json_str_mtx_mtx;  // 评估记录主锁的互斥锁，保护评估记录锁的创建/访问
        map<long long int, mutex> Memorize_the_assessment_records_mtx;  // 按比赛ID细分的评估记录锁，粒度更细的并发控制
        map<long long int, mutex>  Memorize_the_assessment_records_json_str_mtx;
        map<long long int, mutex>  nowcoder_contest_set_json_str_mtx;
        mutex ac_nowcoder_Ranking_data_cmp_mtx;  // 排名数据比较操作锁，保证排名比较的原子性
        mutex nowcoder_contest_list_mtx;  // 比赛列表全局锁，保护比赛列表的读写
        mutex nowcoder_contest_list_max_submissionId_mtx;  // 最大提交ID锁，保证提交ID更新的原子性
        mutex Memorize_the_assessment_records_max_submissionId_mtx;  // 评估记录最大提交ID锁
        mutex Listen_to_the_competition_queue_mtx;  // 监听队列锁，保护监听队列的操作
        mutex Stop_monitoring_the_competition_mtx;  // 停止监听锁，协调监听停止操作
        mutex Contest_Info_map_mtx;  // 比赛信息映射锁，保护比赛元数据
        map<long long int, mutex> nowcoder_contest_set_mtx;  // 按比赛ID分组的排序集合锁
        mutex nowcoder_contest_set_mtx_mtx;  // 排序集合锁的互斥锁
        mutex Unrecorded_Assessment_Status_Map_mtx;  // 状态过滤映射锁
        mutex Memorize_the_assessment_records_Supplementary_order_mtx_mtx;  // 补充订单主锁的互斥锁
        map<long long int, mutex> Memorize_the_assessment_records_Supplementary_order_mtx;  // 按比赛ID细分的补充订单锁
        map<long long int, mutex> nowcoder_contest_map_mtx;  // 按比赛ID细分的排名数据锁
        mutex nowcoder_contest_map_mtx_mtx;  // 排名数据锁的互斥锁
        map<long long int, mutex> Time_of_sealing_Submit_Map_mtx; // 封榜提交锁
        mutex Time_of_sealing_Submit_Map_mtx_mtx; // 封榜提交主锁

        /**
         * @brief 核心请求分发器
         * @param httpRequestData 包含HTTP请求的所有相关信息
         * @param httpResponseData 用于填充HTTP响应的数据结构
         * @return HTTP状态码，表示处理结果
         */
        int Task_distribution_center(HTTP_request_data httpRequestData, HTTP_response_data &httpResponseData);

        /**
         * @brief 获取比赛评估数据
         * @param contestId 唯一标识比赛的ID
         * @param cookie 用于认证的用户凭证
         * @return 操作状态码，成功为0，失败为非0
         */
        int Get_Evaluation_Data(long long int contestId, string cookie);
        int Update_Memorize_the_assessment_records_str(long long int contest_id);
        /**
         * @brief 分页获取评估数据
         * @param contestId 比赛ID
         * @param cookie 认证cookie
         * @param page 请求的页码
         * @param max_pageSize 每页最大记录数
         * @return 包含评估数据的向量，按时间排序
         */
        vector<Evaluation_Data_Template> Get_Evaluation_Data_Single_request(
            long long int contestId, string cookie, int page, int max_pageSize);

        // 核心数据结构
        map<long long int, Contest_Info_Template> Contest_Info_map;  // 比赛元数据存储
        map<long long int, map<long long int, Evaluation_Data_Template, Evaluation_Data_cmp> >
            Memorize_the_assessment_records;  // 评估记录缓存，按比赛ID和提交ID组织
        map<long long int,string> Memorize_the_assessment_records_json_str;//评估记录缓存，按比赛ID和提交ID组织
        map<long long int, map<long long int, int, Evaluation_Data_cmp> >
            Memorize_the_assessment_records_Supplementary_order;  // 待处理评估记录
        map<long long int, long long int> Memorize_the_assessment_records_max_submissionId;  // 各比赛最大提交ID跟踪
        map<long long int, map<long long int, ac_nowcoder_Ranking_data> > nowcoder_contest_map;  // 用户排名数据存储
        map<long long int, set<ac_nowcoder_Ranking_data> > nowcoder_contest_set;  // 排序后的排名集合
        map<long long int, string>nowcoder_contest_set_json_str;
        map<long long int, long long int> nowcoder_contest_list_max_submissionId;  // 榜单最大提交ID
        queue<Listen_to_the_competition_Template> Listen_to_the_competition_queue;  // 待监听比赛队列
        map<long long int, long long int> Stop_monitoring_the_competition;  // 待停止监听比赛列表
        map<string, int> Unrecorded_Assessment_Status_Map;  // 需要过滤的评估状态
        map<long long int, map<long long int, int, Evaluation_Data_cmp> >
            Time_of_sealing_Submit_Map;  // 封榜后提交记录

        /**
         * @brief 榜单更新核心逻辑
         *
         * 执行以下操作：
         * 1. 从队列获取待更新比赛
         * 2. 加载最新评估数据
         * 3. 计算最新排名
         * 4. 更新缓存数据
         */
        void List_update();

        /**
         * @brief 榜单更新任务协调器
         *
         * 职责：
         * 1. 监控监听队列
         * 2. 分配更新任务
         * 3. 处理异常情况
         * 4. 维持更新循环
         */
        void List_Update_Distribution_Center();

        /**
         * @brief 单比赛榜单更新
         * @param contestId 需要更新的比赛ID
         * @return 操作状态码
         */
        int nowcoder_contest_list_update(long long int contestId);

        /**
         * @brief 排名数据更新器
         * @param contestId 比赛ID
         * @param contest_info_template 比赛元数据
         * @param evaluation_data_template 评估数据
         * @param Supplementary_order 是否为补充更新
         * @return 操作状态码
         */
        int update_ac_nowcoder_ranking_data(long long int contestId,Contest_Info_Template contest_info_template,
                                          Evaluation_Data_Template evaluation_data_template,bool Supplementary_order);
        int update_nowcoder_contest_set_json_str(long long int contestId);

        Thread_Pool::Thread_Pool ac_nowcoder_rankings_server_thread_pool;  // 任务执行池
        int Shutdown_bj = 0;  // 服务器状态标志

        /**
         * @brief 榜单生成器
         * @param contest 比赛ID字符串
         * @return 格式化后的榜单HTML/JSON
         */
        string get_nowcoder_contest_list(long long int contestId);
        string get_nowcoder_contest_info(long long int contestId);

        /**
         * @brief 添加新比赛
         * @param contestId 比赛ID
         * @param cookie 认证cookie
         * @param sealing_Status_code 封榜状态码
         * @param Time_of_sealing 封榜时间(ms)
         * @param Time_of_release 解封时间(ms)
         * @return 操作状态码
         */
        int add_Contest_Info(long long int contestId, string cookie,int sealing_Status_code,long long int Time_of_sealing,long long int Time_of_release);

        /**
         * @brief 时间字符串转换器
         * @param time_str 格式为"YYYY-MM-DD HH:MM:SS"的时间字符串
         * @return 毫秒级时间戳，解析失败返回-1
         */
        long long convert_time_string_to_ms(const std::string& time_str) {
            Time_data td;
            if (sscanf(time_str.c_str(), "%d-%d-%d %d:%d:%d",
                       &td.year, &td.month, &td.day,
                       &td.hour, &td.minute, &td.second) != 6) {
                // 处理解析失败
                return -1;
                       }

            tm tm_time = {};
            tm_time.tm_year = td.year - 1900;
            tm_time.tm_mon = td.month - 1;
            tm_time.tm_mday = td.day;
            tm_time.tm_hour = td.hour;
            tm_time.tm_min = td.minute;
            tm_time.tm_sec = td.second;
            tm_time.tm_isdst = -1; // 自动判断夏令时

            time_t sec = mktime(&tm_time);
            return static_cast<long long>(sec) * 1000;
        }
    };
} // ac_nowcoder_rankings_server
// 下一个版本计划实现Evaluation_Data_Template的记忆化互通
#endif //AC_NOWCODER_RANKINGS_SERVER_H
