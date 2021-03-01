#include "datasink.h"
#include <algorithm>

namespace rtcpp {

MemoryDataSink::MemoryDataSink(uint8_t* p, size_t len)
    : buf_(p)
    , len_(len)
    , offset_(0)
{
}

bool MemoryDataSink::write(uint8_t* data, size_t size) {
    size_t bytesToWrite = std::min(size, len_ - offset_);
    if (bytesToWrite > 0) {
        memcpy(buf_ + offset_, data, bytesToWrite);
        offset_ += bytesToWrite;
    }

    return bytesToWrite <= size;
}

FileDataSink::FileDataSink(const std::string& fileName) {
    file_.open(fileName, std::ios::binary);
    if (!file_) {
        throw std::runtime_error("Failed to open file for writing");
    }
}

bool FileDataSink::write(uint8_t* data, size_t size) {
    if (file_.is_open()) {
        file_.write((char*)data, size);
    }
    return file_.good();
}

}