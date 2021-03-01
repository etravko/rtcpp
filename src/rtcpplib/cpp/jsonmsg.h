//
// Created by malkov on 29.01.2021.
//

#ifndef RTCPP_JSONMSG_H
#define RTCPP_JSONMSG_H

#include <string>
#include "rtpanalyzer.h"
#include "transformcfg.h"

namespace rtcpp {
    std::string BuildJSONAnalyzeReport(const RTPStreamAnalyzer& stats);

    void ParseJSONTransformCfg(const std::string& msg, TransformCfg& transformCfg);
}

#endif //RTCPP_JSONMSG_H
