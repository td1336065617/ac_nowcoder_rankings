cmake_minimum_required(VERSION 3.31)
project(ac_nowcoder_rankings)
set(CMAKE_CXX_STANDARD 20)
add_executable(ac_nowcoder_rankings main.cpp
        ac_nowcoder_rankings_server/ac_nowcoder_Ranking_data/ac_nowcoder_Ranking_data.cpp
        ac_nowcoder_rankings_server/ac_nowcoder_Ranking_data/ac_nowcoder_Ranking_data.h
        ac_nowcoder_rankings_server/Contest_Info_Template/Contest_Info_Template.cpp
        ac_nowcoder_rankings_server/Contest_Info_Template/Contest_Info_Template.h
        "ac_nowcoder_rankings_server/Evaluation Data Template/Evaluation_Data_Template.cpp"
        "ac_nowcoder_rankings_server/Evaluation Data Template/Evaluation_Data_Template.h"
        ac_nowcoder_rankings_server/get_nowcoder_contest_list/get_nowcoder_contest_list.cpp
        ac_nowcoder_rankings_server/List_Update/List_update.cpp
        ac_nowcoder_rankings_server/List_Update/List_Update_Distribution_Center.cpp
        ac_nowcoder_rankings_server/List_Update/nowcoder_contest_list_update.cpp
        ac_nowcoder_rankings_server/Listen_to_the_competition_Template/Listen_to_the_competition_Template.cpp
        ac_nowcoder_rankings_server/Listen_to_the_competition_Template/Listen_to_the_competition_Template.h
        ac_nowcoder_rankings_server/tools_class/get_time_ms.cpp
        ac_nowcoder_rankings_server/tools_class/time_class.cpp
        ac_nowcoder_rankings_server/tools_class/time_class.h
        ac_nowcoder_rankings_server/ac_nowcoder_rankings_server.cpp
        ac_nowcoder_rankings_server/ac_nowcoder_rankings_server.h
        ac_nowcoder_rankings_server/add_Contest_Info.cpp
        ac_nowcoder_rankings_server/Get_Evaluation_Data.cpp
        ac_nowcoder_rankings_server/Get_Evaluation_Data_Single_request.cpp
        ac_nowcoder_rankings_server/Task_distribution_center.cpp
        include/Thread_Pool/Thread_Pool.h
        include/HTTP_Server_Framework/HTTP_Server_Framework.h
        ac_nowcoder_rankings_server/List_Update/update_ac_nowcoder_ranking_data.cpp
        ac_nowcoder_rankings_server/get_nowcoder_contest_info/get_nowcoder_contest_info.cpp
        ac_nowcoder_rankings_server/Update_Memorize_the_assessment_records_str.cpp
)
target_include_directories(ac_nowcoder_rankings PRIVATE include)
target_link_directories(ac_nowcoder_rankings PRIVATE lib)
target_link_libraries(ac_nowcoder_rankings PRIVATE libHTTP_Server_Framework.so libThread_Pool.so)