//
// Created by fsptd on 25-5-15.
//

#ifndef TIME_CLASS_H
#define TIME_CLASS_H
#include <sys/time.h>
#include <cstdio>

namespace ac_nowcoder_rankings_server {
    class time_class {
    public:
        long long int get_time_ms();
    };
}


#endif //TIME_CLASS_H
