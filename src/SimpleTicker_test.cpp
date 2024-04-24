#include "SimpleTicker.cpp"

#include <gtest/gtest.h>

TEST(SimpleTickerTest, WaitFunction) {
  const int packets_per_second = 10000;
  const int iterations = 10001;

  auto curre_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
  auto start_time = curre_time + std::chrono::seconds(1); // 下一次开始时间从整秒开始

  std::unique_ptr<SimpleTicker> ticker(new SimpleTicker(start_time,packets_per_second));
  for (int i = 0; i < iterations; i++) {
    ticker->wait();
  }

  auto end_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
  auto diff_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
  EXPECT_EQ(diff_time, 1000000);
}

TEST(SimpleTickerTest, WaitFunction_10001) {
  const int packets_per_second = 10001;
  const int iterations = 10002;

  auto curre_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
  auto start_time = curre_time + std::chrono::seconds(1); // 下一次开始时间从整秒开始

  std::unique_ptr<SimpleTicker> ticker(new SimpleTicker(start_time, packets_per_second));

  for (int i = 0; i < iterations; i++) {
    ticker->wait();
  }
  auto end_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
  auto diff_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

  EXPECT_EQ(diff_time, 1000000);
}

TEST(SimpleTickerTest, WaitFunction_9000) {
  const int packets_per_second = 9000;
  const int iterations = 9001;

  auto curre_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
  auto start_time = curre_time + std::chrono::seconds(1); // 下一次开始时间从整秒开始

  std::unique_ptr<SimpleTicker> ticker(new SimpleTicker(start_time, packets_per_second));
  for (int i = 0; i < iterations; i++) {
    ticker->wait();
  }
  auto end_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
  auto diff_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

  EXPECT_EQ(diff_time, 1000000);
}

TEST(SimpleTickerTest, WaitFunction_Packet_order) {
  const int packets_per_second = 20000;
  const int iterations = 20000;

  auto curre_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
  auto start_time = curre_time + std::chrono::seconds(1); // 下一次开始时间从整秒开始

  std::unique_ptr<SimpleTicker> ticker(new SimpleTicker(start_time,packets_per_second));
  for (int i = 0; i < iterations; i++) {
    auto result = ticker->wait();
    EXPECT_EQ(result.second, i);
  }
}


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  if (result != 0) {
    std::cout << "result:  " << result << std::endl;
  }
  // 获取失败的断言数量
  int num_failures = ::testing::UnitTest::GetInstance()->failed_test_count();
  std::cout << "Number of failed assertions: " << num_failures << std::endl;
}