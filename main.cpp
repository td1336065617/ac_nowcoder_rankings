#include <iostream>
#include "ac_nowcoder_rankings_server/ac_nowcoder_rankings_server.h"
int main() {
    
    ac_nowcoder_rankings_server::ac_nowcoder_rankings_server ac_nowcoder_rankings_server1;

    ac_nowcoder_rankings_server1.set_Thread_Poll(100,5,10);
    ac_nowcoder_rankings_server1.set_server("0.0.0.0",9997);
    //

    ac_nowcoder_rankings_server1.start_server();

}