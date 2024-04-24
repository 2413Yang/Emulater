#ifndef MULTICAST_H
#define MULTICAST_H

#include <stdint.h>
#include <utility>
#include "entity.h"

// 定义节奏器接口
class Ticker {
public:
  virtual ~Ticker() = default;
  // 等待一定时间间隔，返回当前时间秒数
  virtual std::pair<int64_t, int32_t> wait() = 0;
};

// 定义 MessageFormatter 接口
class MessageFormatter {
public:
  virtual void format(uint8_t *data, int64_t time, int32_t order) = 0;
  virtual ~MessageFormatter() = default;
};

// 定义随机数接口
class Random {
public:
  virtual ~Random() = default;
  // 获取随机数
  virtual double random() = 0;
  virtual double quantile() = 0;
};

class Service {
public:
  virtual void serve() = 0;
  virtual void stop() = 0;
};

class Config{
public:
  virtual const std::vector<EmulatorCfg>& load() const = 0;
};
#endif // MULTICAST_H