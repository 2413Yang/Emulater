#include "HFMessageFormatter.cpp"

#include <gtest/gtest.h>

TEST(HFMessageFormatterTest, FormatFUNCTION) {
  uint32_t PacketIdentifier = 0xaa551103;
  std::vector<uint8_t> buffer(2880, 'a');
  auto dataFormat = createHFMessageFormatter(std::move(buffer));

  uint8_t data_one[2988] = {0};
  uint8_t data_two[2988] = {0};

  auto curre_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
  auto micro_time = std::chrono::duration_cast<std::chrono::microseconds>(curre_time).count();
  int32_t order = 1;

  dataFormat->format(data_one, micro_time, order);
  dataFormat->format(data_two, micro_time, order);

  uint32_t packetIdentifier_res = ntohl(*(const uint32_t *)data_one);
  EXPECT_EQ(packetIdentifier_res, PacketIdentifier);

  uint32_t order_res = *(const uint32_t *)(data_one + 4);
  EXPECT_EQ(order_res, htonl(order));

  EXPECT_EQ(memcmp(data_one, data_two, sizeof(data_one)), 0);
}


TEST(HFMessageFormatterTest, CreateHFMessageFormatterFUNCTION) {
  std::vector<uint8_t> buffer(2800, 'a');  //2880
  auto dataFormat = createHFMessageFormatter(std::move(buffer));
  EXPECT_EQ(dataFormat, nullptr);
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