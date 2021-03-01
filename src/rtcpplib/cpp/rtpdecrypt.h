#ifndef RTCPP_RTPDECRYPT_H
#define RTCPP_RTPDECRYPT_H

#include <vector>
#include <set>
#include "srtp2/srtp.h"
#include "transformcfg.h"

namespace rtcpp {

class RTPDecrypt {
public:
    explicit RTPDecrypt(TransformCfg& cfg);
    int Decrypt(uint32_t ssrc, void* p, uint32_t len);

private:
    std::vector<srtp_policy_t> policies_;
    srtp_t session_;
    std::set<uint32_t> ssrcs_;

private:
    void InitPolicy(uint32_t ssrc, const StreamTransformCfg& cfg);
    void InitPolicyCrypto(const StreamTransformCfg& cfg, srtp_policy_t& policy);
};

}

#endif //RTCPP_RTPDECRYPT_H
