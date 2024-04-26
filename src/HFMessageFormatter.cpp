#ifndef HFDATAFORMATTER_H
#define HFDATAFORMATTER_H

#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <ctime>
#include <vector>
#include <memory>
#include "Factory.h"
#include "MessageFormatter.h"

constexpr int32_t HFPacketSize = 2988;  //高频支持的包大小

constexpr uint32_t HFBufferMinSize = 2880;

constexpr uint32_t HFPacketIdentifier = 0xaa551103; // 高频标识符 ( host )

constexpr uint32_t PacketOrderOffset = 4;   // 包序号的偏移量
constexpr uint32_t BodyOffset = 8;          // 数据体部分位的偏移量
constexpr uint32_t HFBeidouTimeOffset = 2888; // 北斗时间的偏移量(高频)

constexpr uint32_t HFBlockSizeFromBuffer = 2880; // 从buffer获取的块的大小(高频)

const static char beidoutimestamp_template[] = {
    0x24,   0x47,   0x50,   0x52,   0x4d,   0x43,   0x2c,   0x30,   0x38,
    0x32,   0x37,   0x32,   0x34,   0x2c,   0x41,   0x2c,   0x2c,   0x2c,
    0x2c,   0x2c,   0x2c,   0x2c,   0x32,   0x34,   0x30,   0x38,   0x32,
    0x33,   0x2c,   0x2c,   0x2a,   0x32,   0x32,   0x0d,   0x0a,   0x24,
    0x47,   0x50,   0x5a,   0x44,   0x41,   0x2c,   0x30,   0x38,   0x32,
    0x37,   0x32,   0x34,   0x2c,   0x32,   0x34,   0x2c,   0x30,   0x38,
    0x2c,   0x32,   0x30,   0x32,   0x33,   0x2c,   0x2c,   0x2a,   0x34,
    0x45,   0x0d,   0x0a,   '\x86', '\x00', '\x98', '\x1e', '\x00', '\x98',
    '\xf8', '\x98', '\xe6', '\x98', '\xe6', '\x18', '\x9e', '\x18', '\x78',
    '\x06', '\x1e', '\xe0', '\xc2', '\x00', '\x18', '\xf8', '\x18', '\x9e',
    '\x18', '\x78', '\x06', '\x3c', '\xc3', '\x3c', '\xf3', '\x3c', '\x03',
    '\x86'};

// 高频 DataFormatter 实现类
class HFMessageFormatter : public MessageFormatter {
public:
  HFMessageFormatter(const std::vector<uint8_t> &buffer) : body_buffer_(buffer), buffer_offset_(0), buffer_size_(body_buffer_.size()) {}

  HFMessageFormatter(std::vector<uint8_t> &&buffer) : body_buffer_(std::move(buffer)), buffer_offset_(0), buffer_size_(body_buffer_.size()) {}

  virtual void format(uint8_t *data, std::int64_t time, int32_t order) override {
    // 设置数据包的标识符和序号（网络字节序）
    *(uint32_t *)data = htonl(HFPacketIdentifier);
    *(uint32_t *)(data + PacketOrderOffset) = htonl(static_cast<uint32_t>(order));

    // 设置数据包data部分
    memcpy(data + BodyOffset, body_buffer_.data() + buffer_offset_, HFBlockSizeFromBuffer);
    buffer_offset_ += HFBlockSizeFromBuffer;
    buffer_offset_ = (buffer_size_ - buffer_offset_) >= HFBlockSizeFromBuffer ? buffer_offset_ : 0;

    // 格式化时间戳
    char hmsdmy[14];
    std::tm *timeinfo = std::localtime(&time);
    std::strftime(hmsdmy, sizeof(hmsdmy), "%H%M%S %d%m%y", timeinfo);
    // 设置时间戳部分
    // int offsetbd = 4 + 4 + (4 * 4 + 4 * 68) * 10;
    memcpy(data + HFBeidouTimeOffset, beidoutimestamp_template, 100);
    memcpy(data + HFBeidouTimeOffset + 7, hmsdmy, 6);
    memcpy(data + HFBeidouTimeOffset + 22, hmsdmy + 7, 6);
    *(int64_t*)(data + HFBeidouTimeOffset + 92) = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  }

private:
  std::vector<uint8_t> body_buffer_; // 数据体缓冲区
  uint32_t buffer_offset_;      // 数据缓冲区偏移量
  int32_t buffer_size_;         // 数据缓冲区大小
};

std::unique_ptr<MessageFormatter> createHFMessageFormatter(std::vector<uint8_t> &&buffer) {
  if (buffer.size() < HFBufferMinSize){
    return nullptr;
  }
  return std::make_unique<HFMessageFormatter>(std::move(buffer));
}

int32_t getHFPacketSize(){
  return HFPacketSize;
}
#endif //HFDATAFORMATTER_H