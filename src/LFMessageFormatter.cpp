#ifndef LFDATAFORMATTER_H
#define LFDATAFORMATTER_H

#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <ctime>
#include <vector>
#include <memory>

#include "Factory.h"
#include "MessageFormatter.h"

constexpr int32_t LFPacketSize = 3204;  //低频包支持的大小

constexpr uint32_t LFBufferMinSize = 3072; // 512 * 6

constexpr uint32_t LFPacketIdentifier = 0x55aa1103; // 低频标识符 ( host )
constexpr uint32_t PacketOrderOffset = 4;   // 包序号的偏移量

constexpr uint32_t BodyOffset = 8;          // 数据体部分位的偏移量

constexpr uint32_t LFCableNumSize = 4;         //低频缆线序号所占字节数
constexpr uint32_t LFBlockSizeFromBuffer = 512; // 从buffer获取的块的大小(低频)
constexpr uint32_t LFBeidouTimeOffset = 3104; // 北斗时间的偏移量(低频)

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


// 低频实现类
class LFMessageFormatter : public MessageFormatter
{
public:
  LFMessageFormatter(std::vector<uint8_t> &buffer) : body_buffer_(buffer), buffer_offset_(0), buffer_size_(body_buffer_.size()) {}
  LFMessageFormatter(std::vector<uint8_t> &&buffer) : body_buffer_(std::move(buffer)), buffer_offset_(0), buffer_size_(body_buffer_.size()) {}

  virtual void format(uint8_t *data, int64_t time, int32_t order) {
    int cable_num_offset = 0;    //传感缆序号
    int body_offset = 0;         
    //设置标识符和包序号
    *(uint32_t*)data = htonl(LFPacketIdentifier);
    *(uint32_t*)(data + PacketOrderOffset) = htonl(static_cast<uint32_t>(order));
    
    //设置消息体部分
    for (int i = 0; i < 6; i++){
      cable_num_offset = BodyOffset + i * (LFCableNumSize + LFBlockSizeFromBuffer);
      body_offset = cable_num_offset + LFCableNumSize;

      *(uint32_t*)(data + cable_num_offset) = htonl(static_cast<uint32_t>(i));
      memcpy(data + body_offset, body_buffer_.data() + buffer_offset_, LFBlockSizeFromBuffer);
      
      buffer_offset_ += LFBlockSizeFromBuffer;
      buffer_offset_ = (buffer_size_ - buffer_offset_) >= LFBlockSizeFromBuffer ? buffer_offset_ : 0;
    }

    //格式化时间戳
    char hmsdmy[14];
    std::tm *timeinfo = std::localtime(&time);
    std::strftime(hmsdmy, sizeof(hmsdmy), "%H%M%S %d%m%y",timeinfo);
    //设置时间戳部分
    memcpy(data + LFBeidouTimeOffset, beidoutimestamp_template, 100);
    memcpy(data + LFBeidouTimeOffset + 7, hmsdmy, 6);
    memcpy(data + LFBeidouTimeOffset + 22, hmsdmy + 7, 6);
  }

private:
  std::vector<uint8_t> body_buffer_; // 存消息体body部分的缓冲区
  uint32_t buffer_offset_;      // 数据缓冲区偏移量
  int32_t buffer_size_;         // 数据缓冲区大小
};

std::unique_ptr<MessageFormatter> createLFMessageFormatter(std::vector<uint8_t> &&buffer){
  if (buffer.size() < LFBufferMinSize){
    return nullptr;
  }
  return std::make_unique<LFMessageFormatter>(std::move(buffer));
}

int32_t getLFPacketSize(){
  return LFPacketSize;
}

#endif //LFDATAFORMATTER_H