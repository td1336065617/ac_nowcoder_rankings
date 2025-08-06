//
// Created by fsptd on 2025/8/6.
//
#include <string>
#include <map>
#include "../Evaluation Data Template/Evaluation_Data_Template.h"
#include "../ac_nowcoder_rankings_server.h"
#include "../tools_class/time_class.h"
#include"nlohmann/json.hpp"
namespace ac_nowcoder_rankings_server {
    /**
     * @brief 获取指定比赛的用户信息并以JSON格式返回
     *
     * 该函数通过比赛ID获取对应的用户信息，包括用户ID、用户名、排名、AC题目数、最后AC时间以及罚时，
     * 并将这些信息组织成JSON格式字符串返回。在访问共享数据时使用了互斥锁来保证线程安全。
     *
     * @param contestId 比赛ID
     * @return string 返回包含用户信息的JSON字符串
     */
    string ac_nowcoder_rankings_server::get_nowcoder_user_info(long long int contestId) {
        string ac_nowcoder_user_info_json_str; // 存储最终生成的JSON字符串
        nlohmann::json ac_nowcoder_user_info_json; // 用于构建JSON数据的对象
        try {
            // 加锁以确保对共享资源 nowcoder_contest_map 的线程安全访问
            nowcoder_contest_map_mtx[contestId].lock();

            // 将比赛ID添加到JSON中
            ac_nowcoder_user_info_json["contestId"] = contestId;

            // 添加参赛用户数量
            ac_nowcoder_user_info_json["size"] = nowcoder_contest_map[contestId].size();

            // 遍历该比赛的所有用户信息，并添加到JSON对象中
            for (auto &item: nowcoder_contest_map[contestId]) {
                // 用户ID
                ac_nowcoder_user_info_json[item.first]["userid"] = item.first;
                // 用户名
                ac_nowcoder_user_info_json[item.first]["username"] = item.second.username;
                // 排名
                ac_nowcoder_user_info_json[item.first]["ranking"] = item.second.ranking;
                // AC题目数
                ac_nowcoder_user_info_json[item.first]["acceptedCount"] = item.second.acceptedCount;
                // 最后一次AC的时间
                ac_nowcoder_user_info_json[item.first]["ac_Finally_time"] = item.second.ac_Finally_time;
                // 罚时
                ac_nowcoder_user_info_json[item.first]["penalty_Time"] = item.second.penalty_Time;
            }

            // 将构建好的JSON对象转换为字符串
            ac_nowcoder_user_info_json_str = ac_nowcoder_user_info_json.dump();
        } catch (...) {
            // 异常处理：确保即使发生异常也释放锁，防止死锁
            nowcoder_contest_map_mtx[contestId].unlock();
            throw; // 重新抛出捕获的异常
        }

        // 正常流程下解锁
        nowcoder_contest_map_mtx[contestId].unlock();

        // 返回包含用户信息的JSON字符串
        return ac_nowcoder_user_info_json_str;
    }

}