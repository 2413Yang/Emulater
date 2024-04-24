#ifndef TICKER_H
#define TICKER_H

#include <thread>

#include "Factory.h"

constexpr int MICROSECONDS = 1000000; // 1000000 us = 1s

static int64_t getCurrentMicroseconds() {
  return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

static void microSleep(int64_t usInterval) {
  int64_t startTime = getCurrentMicroseconds();
  int64_t endTime = startTime + usInterval;
  if (usInterval > 60) {
    std::this_thread::sleep_for(std::chrono::microseconds(usInterval - 58));
  }
  while (getCurrentMicroseconds() < endTime);
}

class SimpleTicker : public Ticker {
public:
  SimpleTicker(std::chrono::seconds start_time, int64_t packets_per_second) : packets_per_second_(packets_per_second) {
    current_second_ = start_time;
    packet_order_ = 0;
  }

  virtual std::pair<int64_t, int32_t> wait() override {
    // 维护下一次醒来时刻
    std::chrono::microseconds next_wake_up_time = std::chrono::microseconds(current_second_.count()*MICROSECONDS + MICROSECONDS * packet_order_ / packets_per_second_);

    // 记录当前时间
    auto current_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
    
    // 当前时间和下一次醒来做对比
    if (current_time < next_wake_up_time) {
      microSleep(std::chrono::duration_cast<std::chrono::microseconds>(next_wake_up_time - current_time).count());
    }

    int64_t return_time = next_wake_up_time.count() / MICROSECONDS;
    int32_t return_packet_order = packet_order_;

    // 包序号自增
    packet_order_++;
    packet_order_ %= packets_per_second_;

    if (packet_order_ == 0) {
      current_second_ += std::chrono::seconds(1);
    }
    // 返回时间戳和包序号
    return {return_time, return_packet_order};
  }

private:
  std::chrono::seconds current_second_;    // 当前秒
  int64_t packet_order_;                   // 包序号
  int64_t packets_per_second_;             // 包发送频率
};

std::unique_ptr<Ticker> createTicker(std::chrono::seconds start_time,int64_t packets_per_second) {
  return std::make_unique<SimpleTicker>(start_time, packets_per_second);
}

#endif