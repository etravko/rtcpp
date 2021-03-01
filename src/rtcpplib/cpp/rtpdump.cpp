#include "rtpdump.h"

#ifdef _WIN32
#include "winsock.h"
#pragma comment(lib, "Ws2_32.lib")
#else
#include "netinet/in.h"
#endif

using namespace rtcpp;

RTPDumpPacketHeaderView::RTPDumpPacketHeaderView(RTPDumpPacketHeader& hdr)
    : hdr_(hdr)
{
}

uint16_t RTPDumpPacketHeaderView::GetLength() {
    return ntohs(hdr_.length_net);
}

void RTPDumpPacketHeaderView::SetLength(uint16_t len) {
    hdr_.length_net = htons(len);
}

uint16_t RTPDumpPacketHeaderView::GetPayloadLen() {
    return ntohs(hdr_.payloadLen_net);
}

void RTPDumpPacketHeaderView::SetPayloadLen(uint16_t plen) {
    hdr_.payloadLen_net = htons(plen);
}

void RTPDumpReader::ReadMagic() {
    const static std::string START_LINE = "#!rtpplay1.0 ";

    if (reader_.ok()) {
        std::string fileStart(START_LINE.size(), '\0');

        auto [pData, dataLen] = reader_.read(fileStart.size());

        if (!reader_.ok() || (dataLen < fileStart.size())) {
            throw std::runtime_error("Failed to read dump header, file is too short");
        }

        memcpy(fileStart.data(), pData, dataLen);

        if (fileStart != START_LINE) {
            throw std::runtime_error("Failed to read dump header, file header mismatch");
        }

        std::vector<uint8_t> fileMagic;
        fileMagic.assign(fileStart.cbegin(), fileStart.cend());

        // read till line end
        uint8_t chr = 0;
        while (reader_.read(chr) && reader_.ok() && (chr != '\n')) {
            fileMagic.push_back(chr);
        }

        if (chr != '\n') {
            throw std::runtime_error("Failed to read dump header, new line isn't found");
        }
        fileMagic.push_back(chr);

        fileMagic_ = std::move(fileMagic);

    } else {
        throw std::runtime_error("Failed to read dump header, stream isn't good");
    }
}

void RTPDumpReader::ReadHeaderData() {
    if (reader_.ok()) {
        RTPDumpHeader dumpHdr = {};

        auto [pData, dataLen] = reader_.read(sizeof(dumpHdr));
        if (!reader_.ok() || (dataLen < sizeof(dumpHdr))) {
            throw std::runtime_error("Failed to read dump header content, file is too short");
        }

        memcpy(&dumpHdr, pData, dataLen);
        dumpHeader_ = dumpHdr;

    } else {
        throw std::runtime_error("Failed to read dump header content, stream isn't good");
    }
}

void RTPDumpReader::ReadHeader() {
    ReadMagic();
    ReadHeaderData();
}

RTPDumpReader::RTPDumpReader(DataSource &reader)
        : reader_(reader) {
    if (!reader_.ok()) {
        throw std::runtime_error("Input stream is bad");
    }

    ReadHeader();
}

const RTPDumpHeader& RTPDumpReader::GetDumpHeader() const {
    return dumpHeader_;
}

const std::vector<uint8_t>& RTPDumpReader::GetDumpHeaderMagic() const {
    return fileMagic_;
}

void RTPDumpReader::ReadPacket(RTPDumpPacket &pkt) {
    if (reader_.ok()) {
        auto [pHdr, hdrLen] = reader_.read(sizeof(RTPDumpPacketHeader));
        if (!reader_.ok() || (hdrLen < sizeof(RTPDumpPacketHeader))) {
            throw std::runtime_error("Failed to read dump packet header, dump has ended");
        }

        memcpy(&pkt.hdr, pHdr, sizeof(RTPDumpPacketHeader));

        size_t contentLen = ntohs(pkt.hdr.length_net) - sizeof(RTPDumpPacketHeader);

        auto [pData, len] = reader_.read(contentLen);
        pkt.payload = {pData, len};

        if (!reader_.ok() || (pkt.payload.length < contentLen)) {
            throw std::runtime_error("Failed to read dump packet payload, stream is short");
        }

    } else {
        throw std::runtime_error("Failed to read dump packet, stream is bad");
    }
}

bool RTPDumpReader::HasMore() {
    return reader_.hasMore();
}

RTPDumpWriter::RTPDumpWriter(DataSink &writer)
    : writer_(writer) {
}

void RTPDumpWriter::WriteHeader(const std::vector<uint8_t> &magic, const RTPDumpHeader &header) {
    if (!writer_.write((uint8_t*)magic.data(), magic.size())) {
        throw std::runtime_error("Failed to write magic");
    }

    if (!writer_.write((uint8_t *) &header, sizeof(header))) {
        throw std::runtime_error("Failed to write rtp dump header");
    }
}

void RTPDumpWriter::WritePacket(const RTPDumpPacket &pkt) {
    if (!writer_.write((uint8_t*) &pkt.hdr, sizeof(RTPDumpPacketHeader))) {
        throw std::runtime_error("Failed to write rtp dump packet header");
    }

    if (!writer_.write(pkt.payload.pData, pkt.payload.length)) {
        throw std::runtime_error("Failed to write rtp dump packet header");
    }
}
