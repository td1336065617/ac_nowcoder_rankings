//
// Created by fsptd on 25-5-15.
//
#include "time_class.h"
namespace ac_nowcoder_rankings_server {
    long long int time_class::get_time_ms() {
        struct timeval currentTime;
        gettimeofday(&currentTime, NULL);
        return currentTime.tv_sec * 1000 + currentTime.tv_usec / 1000;
    }
}