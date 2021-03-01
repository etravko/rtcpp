#include "datasource.h"

#include <exception>
#include <algorithm>

namespace rtcpp {

MemoryDataSource::MemoryDataSource(uint8_t* p, size_t len)
        : buf_(p)
        , len_(len)
        , offset_(0)
{
    if (!p) {
        throw std::runtime_error("Buffer pointer is null");
    }
}

std::pair<uint8_t*, size_t> MemoryDataSource::read(size_t size)  {
    size_t bytesRead = std::min(len_ - offset_, size);

    uint8_t* pos = buf_ + offset_;
    offset_ += bytesRead;

    return {pos, bytesRead};
}

bool MemoryDataSource::read(uint8_t& byte) {
    size_t bytesRead = std::min(len_ - offset_, (size_t)1);

    if (bytesRead > 0) {
        byte = *(buf_ + offset_);
        offset_++;
    }

    return bytesRead > 0;
}

bool MemoryDataSource::hasMore() {
    return offset_ + 1 < len_;
}

bool MemoryDataSource::ok() {
    return true;
}


FileDataSource::FileDataSource(const std::string& fileName) {
    file_.open(fileName, std::ios::binary);
    if (!file_.is_open()) {
        throw std::runtime_error("Failed to open file");
    }
}

std::pair<uint8_t*, size_t> FileDataSource::read(size_t size) {
    buf_.resize(size);
    file_.read((char*)buf_.data(), size);
    return {buf_.data(), (size_t)file_.gcount()};
}

bool FileDataSource::read(uint8_t& byte) {
    return file_.get((char&)byte).good();
}

bool FileDataSource::hasMore() {
    file_.peek();
    return file_.good();
}

bool FileDataSource::ok() {
    return file_.good();
}

}