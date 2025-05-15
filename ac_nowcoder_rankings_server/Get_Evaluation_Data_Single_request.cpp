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
    // @param page: 获取页码
    // @param pageSize: 单页获取数据量
    // @return: 返回一个包含评估数据的vector
    vector<Evaluation_Data_Template> ac_nowcoder_rankings_server::
     Get_Evaluation_Data_Single_request(string contest,string cookie,int page,int max_pageSize) {
        try {


        vector<Evaluation_Data_Template>Evaluation_Data_Template_list;// 存储评估数据的vector
        time_class time_class1; // 时间类实例，用于获取当前时间戳
        // 构造请求URL，包含比赛ID、页码、页面大小和时间戳
        string url = "https://ac.nowcoder.com/acm-heavy/acm/contest/status-list?token=&id="
                    + contest + "&page="+std::to_string(page)+"&pageSize="+ std::to_string(max_pageSize)+"&searchUserName=&_="
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
        long long int basicUid = j1_data_basicInfo["basicUid"].get<long long int>(); // 获取基础用户ID
        long long int contestId = j1_data_basicInfo["contestId"].get<long long int>(); // 获取比赛ID
        long long int pageCount = j1_data_basicInfo["pageCount"].get<long long int>(); // 获取总页数
        long long int pageSize = j1_data_basicInfo["pageSize"].get<long long int>(); // 获取每页大小
        long long int statusCount = j1_data_basicInfo["statusCount"].get<long long int>(); // 获取状态数量
        string searchUserName = j1_data_basicInfo["searchUserName"].get<string>(); // 获取搜索用户名
        long long int pageCurrent = j1_data_basicInfo["pageCurrent"].get<long long int>(); // 获取当前页码

        // 解析数据部分JSON
        j1_data_data = j1_data["data"];
        for (auto &item: j1_data_data) {
            long long int memory = item["memory"].get<long long int>(); // 获取内存使用
            long long int length = item["length"].get<long long int>(); // 获取代码长度
            std::string index = item["index"].get<string>(); // 获取索引
            std::string language = item["language"].get<string>(); // 获取编程语言
            std::string userName = item["userName"].get<string>(); // 获取用户名
            long long int userId = item["userId"].get<long long int>(); // 获取用户ID
            long long int colorLevel = item["colorLevel"].get<long long int>(); // 获取颜色等级
            std::string statusMessage = item["statusMessage"].get<string>(); // 获取状态消息
            long long int submissionId = item["submissionId"].get<long long int>(); // 获取提交ID
            long long int submitTime = item["submitTime"].get<long long int>(); // 获取提交时间
            long long int time = item["time"].get<long long int>(); // 获取时间
            long long int problemId = item["problemId"].get<long long int>(); // 获取问题ID

            // 创建评估数据模板并存入map
            Evaluation_Data_Template evaluation_data_template(memory, length, index
                ,language, userName, userId
                , colorLevel, statusMessage, submissionId
                , submitTime, time, problemId);
            Evaluation_Data_Template_list.emplace_back(evaluation_data_template);
        }

        return Evaluation_Data_Template_list;
        }catch (exception &e) {
            cout << "Get_Evaluation_Data_Single_request error: 大概率是比赛场次不存在 " << e.what() << endl;
        }
    }
}
