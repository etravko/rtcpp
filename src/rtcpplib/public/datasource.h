#ifndef RTCPP_DATASOURCE_H
#define RTCPP_DATASOURCE_H

#include <cstdint>
#include <cstdlib>
#include <utility>
#include <vector>
#include <iostream>
#include <fstream>

namespace rtcpp {

class DataSource {
public:
    virtual ~DataSource() = default;

    virtual std::pair<uint8_t*, size_t> read(size_t size) = 0;
    virtual bool read(uint8_t& byte) = 0;
    virtual bool hasMore() = 0;
    virtual bool ok() = 0;
};

class MemoryDataSource : public DataSource {
private:
    uint8_t* buf_;
    size_t len_;
    size_t offset_;

public:
    MemoryDataSource(uint8_t* p, size_t len);

    std::pair<uint8_t*, size_t> read(size_t size) override;
    bool read(uint8_t& byte) override;

    bool hasMore() override;
    bool ok() override;
};

class FileDataSource : public DataSource {
private:
    std::ifstream file_;
    std::vector<uint8_t> buf_;

public:
    explicit FileDataSource(const std::string& fileName);

    FileDataSource(const FileDataSource&) = delete;
    FileDataSource& operator=(const FileDataSource&) = delete;

    std::pair<uint8_t*, size_t> read(size_t size) override;
    bool read(uint8_t& byte) override;

    bool hasMore() override;
    bool ok() override;
};

}

#endif //RTCPP_DATASOURCE_H
