#include "RandomDelayTicker.cpp"
#include <gtest/gtest.h>

TEST(RandomTickerTest, WaitFunctionDelay) {
  const int packets_per_second = 20000;
  const int iterations = 10;

  auto curre_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
  auto start_time = curre_time + std::chrono::seconds(1); // 下一次开始时间从整秒开始
  // 80%的累积概率，对应的分位数是1.28155; 代表着20%的丢包率，如果获取到的随机数大于1.28155则丢包
  // 0.2小于1.28155，则代表要发送；
  // 随机数0.2的延迟：0.2 * 51 = 10.2 us,取整10us; 代表着随机到0.2的包在正常发送时刻要延迟10us发送
  std::shared_ptr<Random> random(new SimulateRandomNum(std::vector<double>{0.2,0.2,0.2,0.2,0.2,0.2,0.2,0.2,0.2,0.2},1.28155));
  std::vector<int32_t> expectOrder{0,1,2,3,4,5,6,7,8,9};
  std::vector<int32_t> resultOrder;
  std::unique_ptr<RandomDelayTicker> ticker(new RandomDelayTicker(random, start_time, packets_per_second));
  for (int i = 0; i < iterations; i++) {
    auto res = ticker->wait();
    resultOrder.push_back(res.second);
  }
  EXPECT_EQ(expectOrder,resultOrder);
}


TEST(RandomTickerTest, WaitFunctionPacketLoss) {
  const int packets_per_second = 20000;
  const int iterations = 10;
  auto curre_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
  auto start_time = curre_time + std::chrono::seconds(1); // 下一次开始时间从整秒开始
  // 80%的累积概率，对应的分位数是1.28155; 代表着20%的丢包率，如果获取到的随机数大于1.28155则丢包
  // 1.0小于1.28155，则代表要发送；2.0 大于1.28155，则代表丢包，不发送。
  // 随机数1.0的延迟：1.0 * 51 = 51.0 us; 代表着随机到1.0的包在正常发送时刻要延迟51us发送
  // 包序号为5的包对应的随机数为2.0，该包不发送，模拟丢包
  std::shared_ptr<Random> random(new SimulateRandomNum(std::vector<double>{1.0,1.0,1.0,1.0,1.0,2.0,1.0,1.0,1.0,1.0,1.0,1.0},1.28155));

  std::vector<int32_t> expectOrder{0,1,2,3,4,6,7,8,9,10};
  std::vector<int32_t> resultOrder;
  std::unique_ptr<RandomDelayTicker> ticker(new RandomDelayTicker(random, start_time, packets_per_second));
  for (int i = 0; i < iterations; i++) {
    auto res = ticker->wait();
    resultOrder.push_back(res.second);
  }
  EXPECT_EQ(expectOrder,resultOrder);
  
}

TEST(RandomTickerTest, WaitFunctionOutOfOrder) {
  const int packets_per_second = 20000;
  const int iterations = 10;
  auto curre_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
  auto start_time = curre_time + std::chrono::seconds(1); // 下一次开始时间从整秒开始
  // 99%的累积概率，对应的分位数是2.57583; 代表着1%的丢包率，如果获取到的随机数大于2.57583则丢包
  // 2.5和0.01都小于2.57583，则代表要发送；
  // 随机数0.01的延迟：0.01 * 51 = 0.51 us 取整 0 us; 代表着随机到0.01数的包在正常发送时刻发送
  // 随机数2.5的延迟：2.5 * 51 = 127.5 us 取整 127 us; 代表着随机到2.5数的包在正常发送时刻要延迟127us发送
  std::shared_ptr<Random> random(new SimulateRandomNum(std::vector<double>{2.5,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01},2.57583));

  std::vector<int32_t> expectOrder{1,2,0,3,4,5,6,7,8,9};
  std::vector<int32_t> resultOrder;
  std::unique_ptr<RandomDelayTicker> ticker(new RandomDelayTicker(random, start_time, packets_per_second));
  for (int i = 0; i < iterations; i++) {
    auto res = ticker->wait();
    resultOrder.push_back(res.second);
  }
  EXPECT_EQ(expectOrder,resultOrder);
  
}

TEST(RandomTickerTest, WaitFunctionOutOfOrderAndPacketLoss) {
  const int packets_per_second = 20000;
  const int iterations = 10;
  auto curre_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
  auto start_time = curre_time + std::chrono::seconds(1); // 下一次开始时间从整秒开始
  // 99%的累积概率，对应的分位数是2.57583; 代表着1%的丢包率，如果获取到的随机数大于2.57583则丢包
  // 2.5和0.01都小于2.57583，则代表要发送；3.0 大于2.57583，则代表丢包，不发送。
  // 随机数0.01的延迟：0.01 * 51 = 0.51 us 取整 0 us; 代表着随机到0.01数的包在正常发送时刻发送
  // 随机数2.5的延迟：2.5 * 51 = 127.5 us 取整 127 us; 代表着随机到2.5数的包在正常发送时刻要增加127us的延迟才发送
  // 下面则模拟了1%的丢包率，包序号为0的包对应的随机数是2.5，该包在正常发送时刻要延迟127us发送，模拟乱序;
  // 包序号为5的包对应的随机数是3.0，该包不发送，模拟丢包
  std::shared_ptr<Random> random(new SimulateRandomNum(std::vector<double>{2.5,0.01,0.01,0.01,0.01,3.0,0.01,0.01,0.01,0.01,0.01,0.01,0.01},2.57583));
  std::vector<int32_t> expectOrder{1,2,0,3,4,6,7,8,9,10};
  std::vector<int32_t> resultOrder;
  std::unique_ptr<RandomDelayTicker> ticker(new RandomDelayTicker(random, start_time, packets_per_second));
  for (int i = 0; i < iterations; i++) {
    auto res = ticker->wait();
    resultOrder.push_back(res.second);
  }
  EXPECT_EQ(expectOrder,resultOrder);
}