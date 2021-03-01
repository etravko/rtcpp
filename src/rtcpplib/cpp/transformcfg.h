#ifndef RTCPP_TRANSFORMCFG_H
#define RTCPP_TRANSFORMCFG_H

#include <cstdint>
#include <utility>
#include <map>
#include <set>
#include "encryption.h"

namespace rtcpp {

#define ADD_CFG_OPTION(NAME, TYPE, VARIABLE)               \
    void Set##NAME(TYPE NAME) { VARIABLE = {true, NAME}; } \
    TYPE Get##NAME() const { return VARIABLE.second; }           \
    bool Is##NAME() const { return VARIABLE.first; }

class StreamTransformCfg {
private:
    std::pair<bool, uint32_t> ssrc_;
    std::pair<bool, uint8_t> pt_;
    std::pair<bool, std::vector<uint8_t>> key_;
    std::pair<bool, EncryptionMode> encMode_;
    std::pair<bool, EncryptionType> encType_;

public:
    ADD_CFG_OPTION(SSRC, uint32_t, ssrc_)
    ADD_CFG_OPTION(PT, uint8_t, pt_)
    ADD_CFG_OPTION(EncKey, const std::vector<uint8_t>&, key_)
    ADD_CFG_OPTION(EncMode, EncryptionMode, encMode_)
    ADD_CFG_OPTION(EncType, EncryptionType, encType_)

    bool IsEncryptionEnabled() const {
        return IsEncKey() || IsEncMode() || IsEncType();
    }
};

using TransformCfg = std::map<uint32_t, StreamTransformCfg>;

}
#endif //RTCPP_TRANSFORMCFG_H
