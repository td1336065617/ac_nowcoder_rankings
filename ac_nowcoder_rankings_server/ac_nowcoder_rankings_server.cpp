//
// Created by fsptd on 25-5-14.
//

#include "ac_nowcoder_rankings_server.h"

namespace ac_nowcoder_rankings_server {
    int ac_nowcoder_rankings_server::Task_distribution_center(HTTP_request_data httpRequestData,
        HTTP_response_data &httpResponseData) {
        //获取请求信息，设置返回信息
        if (httpRequestData.url=="/nowcoder_contest_list.jk") {
            httpResponseData.http_response_value="11111";
            httpRequestData.http_data_value["contest"]="108665";
            httpRequestData.HTTP_Cookie="NOWCODERUID=6195106A55ACC9BEB7015EF2294F3900; NOWCODERCLINETID=5523F0D6A425C6FE240C68C6ED0A5DBA; gr_user_id=1dd04216-3fde-4cb3-bb09-1cf807147b65; Hm_lvt_a808a1326b6c06c437de769d1b85b870=1747139928,1747150815,1747287660,1747290027; isAgreementChecked=true; t=CEF7DD92A234C15EABE5C4AD96512FF3; c196c3667d214851b11233f5c17f99d5_gr_last_sent_cs1=645160704; _clck=838x9s%7C2%7Cfvv%7C0%7C1959; Hm_lpvt_a808a1326b6c06c437de769d1b85b870=1747296972; HMACCOUNT=1A9C00CB5176AE5F; c196c3667d214851b11233f5c17f99d5_gr_session_id=183d7d2c-f52d-42ac-99a9-86c14961b4bf; c196c3667d214851b11233f5c17f99d5_gr_last_sent_sid_with_cs1=183d7d2c-f52d-42ac-99a9-86c14961b4bf; c196c3667d214851b11233f5c17f99d5_gr_cs1=645160704; c196c3667d214851b11233f5c17f99d5_gr_session_id_sent_vst=183d7d2c-f52d-42ac-99a9-86c14961b4bf; acw_tc=0a03837b17472969724856697e7566beeacf41227be7e538190424a668d941";
            Get_Evaluation_Data(httpRequestData.http_data_value["contest"],httpRequestData.HTTP_Cookie);
        }
        httpResponseData.http_response_data_value="11111";
        return 0;
    }
} // ac_nowcoder_rankings_server