#include "Sleep.h"
#include <chrono>
#include <thread>

int64_t getCurrentMicroseconds() {
  return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

// 当程序需要进行微秒级别的睡眠时，反复查询时间以实现阻塞会导致 CPU 占用率过高。
// 为了解决这个问题，可以使用 sleep_for 函数进行睡眠，即让出 CPU 使用权。
// 但是sleep_for微秒精度下的睡眠存在一定的误差，具体来说，如果睡眠时间大于最大误差时间，那么程序会睡眠 (usInterval - 最大误差时间) 微秒，以便让出 CPU 给其他任务使用。
// 最大误差时间的值取决于运行当前程序的设备。
void microSleep(int64_t usInterval) {
  int64_t startTime = getCurrentMicroseconds();
  int64_t endTime = startTime + usInterval;
  if (usInterval > 60) {
    std::this_thread::sleep_for(std::chrono::microseconds(usInterval - 60));
  }
  while (getCurrentMicroseconds() < endTime);
}