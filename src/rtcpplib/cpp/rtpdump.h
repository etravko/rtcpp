#ifndef RTCPP_RTPDUMP_H
#define RTCPP_RTPDUMP_H

#include <cstdlib>
#include <cstdint>
#include <vector>
#include "datasource.h"
#include "datasink.h"

namespace rtcpp {

struct RTPDumpHeader {
    uint32_t startSec_net;
    uint32_t startUsec_net;
    uint32_t sourceIP_net;
    uint16_t port_net;
    uint16_t padding;
};

struct RTPDumpPacketHeader {
    uint16_t length_net;
    uint16_t payloadLen_net;
    uint32_t offset_net;
};

class RTPDumpPacketHeaderView {
private:
    RTPDumpPacketHeader& hdr_;
public:
    RTPDumpPacketHeaderView(RTPDumpPacketHeader& hdr);

    uint16_t GetLength();
    void SetLength(uint16_t len);

    uint16_t GetPayloadLen();
    void SetPayloadLen(uint16_t);

    ~RTPDumpPacketHeaderView() = default;
    RTPDumpPacketHeaderView(RTPDumpPacketHeaderView&) = delete;
    RTPDumpPacketHeaderView& operator=(const RTPDumpPacketHeaderView&) = delete;
};

struct RTPDUmpPacketPayload {
    uint8_t* pData;
    size_t length;
};

struct RTPDumpPacket {
    RTPDumpPacketHeader hdr{};
    RTPDUmpPacketPayload payload{};
};


class RTPDumpReader {
private:
    DataSource &reader_;
    RTPDumpHeader dumpHeader_ = {};
    std::vector<uint8_t> fileMagic_;

private:
    void ReadMagic();
    void ReadHeaderData();
    void ReadHeader();

public:
    explicit RTPDumpReader(DataSource &reader);
    ~RTPDumpReader() = default;

    RTPDumpReader(const RTPDumpReader&) = delete;
    RTPDumpReader& operator=(const RTPDumpReader&) = delete;

    const RTPDumpHeader &GetDumpHeader() const;
    const std::vector<uint8_t> &GetDumpHeaderMagic() const;

    void ReadPacket(RTPDumpPacket &pkt);
    bool HasMore();
};

class RTPDumpWriter {
private:
    DataSink &writer_;

public:
    RTPDumpWriter(DataSink &writer);

    RTPDumpWriter(const RTPDumpWriter&) = delete;
    RTPDumpWriter& operator=(const RTPDumpWriter&) = delete;

    void WriteHeader(const std::vector<uint8_t> &magic, const RTPDumpHeader &header);
    void WritePacket(const RTPDumpPacket &pkt);
};


}
#endif //RTCPP_RTPDUMP_H
