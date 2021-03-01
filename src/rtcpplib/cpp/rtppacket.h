#ifndef RTCPP_RTPPACKET_H
#define RTCPP_RTPPACKET_H

#include <cstdint>
#include <vector>
#include "rtpdump.h"

namespace rtcpp {

class RTPPacketView {

private:
    static const uint32_t MIN_RTP_PACKET_LEN = 12;
    uint8_t* payload_;
    size_t payloadLen_;

public:
    explicit RTPPacketView(RTPDUmpPacketPayload &pkt)
            : payload_(pkt.pData)
            , payloadLen_(pkt.length) {
        if (payloadLen_ < MIN_RTP_PACKET_LEN) {
            throw std::runtime_error("Bad packet length");
        }
    }

    uint8_t GetPT() const {
        uint8_t byte = *(payload_ + 1u);
        return byte & 0x7Fu;
    }

    void SetPT(uint8_t pt) {
        uint8_t byte = *(payload_ + 1u) & 0x80u;
        byte = byte | (pt & 0x7Fu);
        *(payload_ + 1u) = byte;
    }

    uint16_t GetSeqNum() const {
        const uint32_t seqOffset = 8;

        uint32_t byte0 = (uint32_t)(*(payload_ + seqOffset + 1u));
        uint32_t byte1 = (uint32_t)(*(payload_ + seqOffset + 0u) << 8u);

        uint32_t seqn = byte0 + byte1;
        return (uint16_t)seqn;
    }

    uint32_t GetTimestamp() const {
        const uint32_t tsOffset = 4;

        uint32_t byte0 = (uint32_t)(*(payload_ + tsOffset + 3u));
        uint32_t byte1 = (uint32_t)(*(payload_ + tsOffset + 2u) << 8u);
        uint32_t byte2 = (uint32_t)(*(payload_ + tsOffset + 1u) << 16u);
        uint32_t byte3 = (uint32_t)(*(payload_ + tsOffset + 0u) << 24u);

        uint32_t ts = byte0 + byte1 + byte2 + byte3;

        return ts;
    }

    uint32_t GetSSRC() const {
        const uint32_t ssrcOffset = 8;

        uint32_t byte0 = (uint32_t)(*(payload_ + ssrcOffset + 0u) << 24u);
        uint32_t byte1 = (uint32_t)(*(payload_ + ssrcOffset + 1u) << 16u);
        uint32_t byte2 = (uint32_t)(*(payload_ + ssrcOffset + 2u) << 8u);
        uint32_t byte3 = (uint32_t)(*(payload_ + ssrcOffset + 3u));

        uint32_t ssrc = byte0 + byte1 + byte2 + byte3;

        return ssrc;
    }
};

}

#endif //RTCPP_RTPPACKET_H
