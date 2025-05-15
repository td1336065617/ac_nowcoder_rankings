//
// Created by fsptd on 25-5-15.
//

#include <string>
#include <map>
#include "Evaluation Data Template/Evaluation_Data_Template.h"
#include "ac_nowcoder_rankings_server.h"
#include "tools_class/time_class.h"
#include"nlohmann/json.hpp"
namespace ac_nowcoder_rankings_server {
    using namespace nlohmann;

    // 获取比赛评估数据
    // @param contest: 比赛ID
    // @param cookie: 用户cookie
    // @return: 返回一个包含评估数据的map，key为提交ID，value为评估数据模板
    std::map<long long int, Evaluation_Data_Template> ac_nowcoder_rankings_server::Get_Evaluation_Data(
        std::string contest, std::string cookie){
        time_class time_class1; // 时间类实例，用于获取当前时间戳
        std::map<long long int, Evaluation_Data_Template> Evaluation_Data_Map; // 存储评估数据的map

        // 构造请求URL，包含比赛ID、页码、页面大小和时间戳
        string url = "https://ac.nowcoder.com/acm-heavy/acm/contest/status-list?token=&id="
                     + contest + "&page=1&pageSize=50&searchUserName=&_="
                     + std::to_string(time_class1.get_time_ms());

        // 发送HTTP请求并获取响应
        string str = epollRequest.get_url(url, cookie);

        // 解析响应JSON
        json j1, j1_data;
        j1 = json::parse(str);
        string msg = j1["msg"].get<string>(); // 获取响应消息
        int code = j1["code"].get<int>(); // 获取响应代码
        j1_data= j1["data"]; // 获取响应数
        bool data_isContestFinished = j1_data["isContestFinished"].get<bool>(); // 获取比赛是否结束标志

        // 解析基本信息部分JSON
        json j1_data_basicInfo, j1_data_data;
        j1_data_basicInfo =  j1_data["basicInfo"]; // 获取基本信息
        int basicUid = j1_data_basicInfo["basicUid"].get<int>(); // 获取基础用户ID
        int contestId = j1_data_basicInfo["contestId"].get<int>(); // 获取比赛ID
        int pageCount = j1_data_basicInfo["pageCount"].get<int>(); // 获取总页数
        int pageSize = j1_data_basicInfo["pageSize"].get<int>(); // 获取每页大小
        int statusCount = j1_data_basicInfo["statusCount"].get<int>(); // 获取状态数量
        string searchUserName = j1_data_basicInfo["searchUserName"].get<string>(); // 获取搜索用户名
        int pageCurrent = j1_data_basicInfo["pageCurrent"].get<int>(); // 获取当前页码
        long long int All_min_submission_id = LLONG_MAX, min_submission_id = LLONG_MAX; // 用于存储最小提交ID的变量
        // 遍历所有页面，获取评估数据
        for (int i = 0; i < pageCount + 10; i++) {
            // 获取单页的评估数据列表
            vector<Evaluation_Data_Template> evaluation_data_templates_list =
                Get_Evaluation_Data_Single_request(contest,cookie, i, 50);
            // 遍历评估数据列表，将数据存入map并更新最小提交ID
            for (auto& evaluation_data_templates : evaluation_data_templates_list) {
                Evaluation_Data_Map[evaluation_data_templates.get_submission_id()] = evaluation_data_templates;
                min_submission_id = min(min_submission_id, evaluation_data_templates.get_submission_id());
            }
            // 如果当前页码超过总页数且最小提交ID未变化，则停止循环
            if (i >= pageCount && All_min_submission_id == min_submission_id) break;
            // 更新全局最小提交ID
            All_min_submission_id = min_submission_id;
        }
        // 输出评估数据的数量
        cout << Evaluation_Data_Map.size() << endl;
        // 返回评估数据map
        return Evaluation_Data_Map;
    }
} // ac_nowcoder_rankings_server
