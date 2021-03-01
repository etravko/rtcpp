//
// Created by malkov on 29.01.2021.
//

#ifndef RTCPP_RTPANALYZER_H
#define RTCPP_RTPANALYZER_H

#include <cstdint>
#include <map>
#include "rtppacket.h"

namespace rtcpp {

struct RtpStreamInfo {
    uint32_t packetCount;
    uint8_t pt;
};

class RTPStreamAnalyzer {
public:
    using SSRC = uint32_t;
    using StreamsStat = std::map<SSRC, RtpStreamInfo>;

private:
    StreamsStat stats_;

public:
    void AnalyzePacket(const RTPPacketView& packet) {
        uint32_t ssrc = packet.GetSSRC();
        auto it = stats_.find(ssrc);

        if (it == stats_.end()) {
            auto [tmpIt, success] = stats_.insert({ssrc, {}});
            it = tmpIt;

            it->second.pt = packet.GetPT();
        }

        RtpStreamInfo& info = it->second;
        info.packetCount++;
    }

    const StreamsStat& Info() const {
        return stats_;
    }

    const uint32_t TotalPackets() const {
        uint32_t totalPackets = 0;
        for (const auto& stream : stats_) {
            totalPackets += stream.second.packetCount;
        }

        return totalPackets;
    }
};

}

#endif //RTCPP_RTPANALYZER_H
