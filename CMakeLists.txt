cmake_minimum_required(VERSION 3.31)
project(ac_nowcoder_rankings)

set(CMAKE_CXX_STANDARD 20)

add_executable(ac_nowcoder_rankings main.cpp
        ac_nowcoder_rankings_server/ac_nowcoder_rankings_server.cpp
        ac_nowcoder_rankings_server/ac_nowcoder_rankings_server.h
        "ac_nowcoder_rankings_server/Evaluation Data Template/Evaluation_Data_Template.cpp"
        "ac_nowcoder_rankings_server/Evaluation Data Template/Evaluation_Data_Template.h"
        ac_nowcoder_rankings_server/Get_Evaluation_Data.cpp
        ac_nowcoder_rankings_server/tools_class/time_class.cpp
        ac_nowcoder_rankings_server/tools_class/time_class.h
        ac_nowcoder_rankings_server/Task_distribution_center.cpp
        ac_nowcoder_rankings_server/tools_class/get_time_ms.cpp
        ac_nowcoder_rankings_server/Get_Evaluation_Data_Single_request.cpp)
target_include_directories(ac_nowcoder_rankings PRIVATE include)
target_link_directories(ac_nowcoder_rankings PRIVATE lib)
target_link_libraries(ac_nowcoder_rankings PRIVATE libHTTP_Server_Framework.so)