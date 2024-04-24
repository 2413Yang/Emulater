#ifndef LOSTPACKETTICKER_H
#define LOSTPACKETTICKER_H

#include "MulticastSender.h"
#include "Factory.h"
#include "Sleep.h"
#include <boost/math/distributions/normal.hpp>
#include <random>
#include <mutex>
#include <thread>
#include <vector>
#include <condition_variable>
#include <map>

constexpr int MICROSECONDS = 1000000; // 1000000 us = 1s
constexpr int DELAYWEIGHT = 51;       // 延迟权重
using namespace std::chrono;

class NormalDistributionRandomNum : public Random {
public:
  NormalDistributionRandomNum(double cumulative_probability) : gen_(std::random_device{}()), dist_(0.0,0.1){
    boost::math::normal_distribution<double> normal(0.0,1.0);
    z_ = boost::math::quantile(normal,(1 + cumulative_probability) / 2);
  }

  virtual double random() override{
    return std::abs(dist_(gen_));
  }

  virtual double quantile() override{
    return z_;
  }
private:
  std::mt19937 gen_;
  std::normal_distribution<double> dist_;  // 正态分布随机数
  double z_;      // 分位数  
};

class SimulateRandomNum : public Random {
public:
  SimulateRandomNum(std::vector<double> &&random_list, const double z):random_list_(std::move(random_list)), z_(z), index_(0){}
  virtual double random() override{
    if(index_ >= random_list_.size()){
      return 0.0;
    }
    return random_list_[index_++];
  }

  virtual double quantile() override{
    return z_;
  }
private:
  std::vector<double> random_list_;
  double z_;
  size_t index_; // 记录当前迭代的索引
};

struct item {
    item(int64_t r, int64_t s, int32_t o):real_send_time(r), second(s), order(o){}
    int64_t real_send_time;
    int64_t second;
    int32_t order;
};

bool item_compare(const item& left, const item& right) {
    return left.real_send_time > right.real_send_time;
}

class MinHeap {
private:
    std::vector<item> _heap;

public:
    void push(item i) {
      _heap.emplace_back(i);
      std::push_heap(_heap.begin(), _heap.end(), item_compare);
    }

    const item& front() const {
      if (_heap.empty()) {
        throw std::out_of_range("Heap is empty!");
      }
      return _heap.front();
    }

    item pop() {
      if (_heap.empty()) {
        throw std::out_of_range("Heap is empty!");
      }
      item min = _heap.front();
      std::pop_heap(_heap.begin(), _heap.end(), item_compare);
      _heap.pop_back();
      return min;
    }

    bool empty() {
        return _heap.empty();
    }
};

class RandomDelayTicker : public Ticker{
public:
  RandomDelayTicker(std::shared_ptr<Random> nd_random,seconds start_time, int64_t packets_per_second)
  : nd_random_(nd_random), current_second_(start_time), packets_per_second_(packets_per_second), packet_order_(0) {}
  virtual std::pair<int64_t,int32_t> wait() override{
      while(true) {
          int64_t next_packet_time_ms = current_second_.count()*MICROSECONDS + MICROSECONDS * packet_order_ / packets_per_second_;
          if (!minHeap_.empty() && minHeap_.front().real_send_time <= next_packet_time_ms) {
              int64_t current_time_ms = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
              if(current_time_ms < minHeap_.front().real_send_time) {
                  microSleep(minHeap_.front().real_send_time - current_time_ms);
              }

              auto send_item =  minHeap_.pop();
              return {send_item.second, send_item.order};
          }
          double random = nd_random_->random();
          if (random < nd_random_->quantile()) {
              int64_t delay_wake_time_ms = next_packet_time_ms + static_cast<int32_t>(random * DELAYWEIGHT);
              minHeap_.push(item(delay_wake_time_ms, current_second_.count(), packet_order_));
          }
          packet_order_++;
          packet_order_ %= packets_per_second_;
          if(packet_order_ == 0){
              current_second_ += seconds(1);
          }
      }
  }
private:
  std::shared_ptr<Random> nd_random_;
  seconds current_second_;    // 当前时间 精度秒
  int64_t packets_per_second_;             // 每秒包频率
  int32_t packet_order_;                   // 包序号
  MinHeap minHeap_;                        // 待发送队列
};


std::shared_ptr<Random> createRandom(double cumulative_probability) {
  return std::shared_ptr<Random> (new NormalDistributionRandomNum(cumulative_probability));
}

std::unique_ptr<Ticker> createPacketLossTicker(std::shared_ptr<Random> random ,seconds start_time, int64_t packets_per_second) {
  return std::make_unique<RandomDelayTicker>(random, start_time, packets_per_second);
}

#endif //LOSTPACKETTICKER_H
