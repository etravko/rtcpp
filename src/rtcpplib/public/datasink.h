#ifndef RTCPP_DATASINK_H
#define RTCPP_DATASINK_H

#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>

namespace rtcpp {

class DataSink {
public:
    virtual ~DataSink() = default;

    virtual bool write(uint8_t* data, size_t size) = 0;
};

class MemoryDataSink : public DataSink {
private:
    uint8_t* buf_;
    size_t len_;
    size_t offset_;

public:
    MemoryDataSink(uint8_t* p, size_t len);
    bool write(uint8_t* data, size_t size) override;

};

class FileDataSink : public DataSink {
private:
    std::ofstream file_;

public:
    explicit FileDataSink(const std::string& fileName);

    FileDataSink(const FileDataSink&) = delete;
    FileDataSink& operator=(const FileDataSink&) = delete;

    bool write(uint8_t* data, size_t size) override;
};

}


#endif //RTCPP_DATASINK_H
