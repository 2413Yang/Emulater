#include <gtest/gtest.h>
#include "EmulaterConfig.cpp"
#include <sstream>

TEST(EmulaterConfigTest, LoadFunction) {
    // 构造一个包含YAML文档的字符串
    std::string yamlDocument = R"(
    Emulators:
      - name: "Emulator1"
        mode: HF
        packets_per_second: 20000
        packet_size: 2988
        data_file: "ToBeSendData_1.bin"
        group_ip: "239.255.0.2"
        group_port: 8888
        ticker:
            ticker_type: SimpleTicker

      - name: "Emulator2"
        mode: LF
        packets_per_second: 5000
        packet_size: 3204
        data_file: "ToBeSendData_2.bin"
        group_ip: "224.81.128.11"
        group_port: 8081
        ticker:
            ticker_type: RandomDelayTicker
            packet_loss: 0.0
    )";

    // 构造字符串流并将YAML文档作为字符串输入
    std::istringstream inputStream(yamlDocument);

    YAML::Node emulators = YAML::Load(inputStream);
    EmulaterConfig config(emulators);

    // 调用 load 方法加载配置信息
    const std::vector<EmulatorCfg>& cfgList = config.load();

    // 检查加载的配置信息是否符合预期
    ASSERT_FALSE(cfgList.empty()); // 确保加载的配置列表不为空
    ASSERT_EQ(cfgList.size(), 2); // 确保加载的配置列表大小为 2
    // 检查第一个配置信息是否符合预期
    EXPECT_EQ(cfgList[0].name, "Emulator1");
    EXPECT_EQ(cfgList[0].mode, "HF");
    EXPECT_EQ(cfgList[0].packets_per_second, 20000);
    EXPECT_EQ(cfgList[0].packet_size, 2988);
    EXPECT_EQ(cfgList[0].data_file, "ToBeSendData_1.bin");
    EXPECT_EQ(cfgList[0].group_ip, "239.255.0.2");
    EXPECT_EQ(cfgList[0].group_port, 8888);
    EXPECT_EQ(cfgList[0].ticker.ticker_type, "SimpleTicker");
    // EXPECT_EQ(cfgList[0].ticker.packet_loss, 0.0);
    // 检查第二个配置信息是否符合预期
    EXPECT_EQ(cfgList[1].name, "Emulator2");
    EXPECT_EQ(cfgList[1].mode, "LF");
    EXPECT_EQ(cfgList[1].packets_per_second, 5000);
    EXPECT_EQ(cfgList[1].packet_size, 3204);
    EXPECT_EQ(cfgList[1].data_file, "ToBeSendData_2.bin");
    EXPECT_EQ(cfgList[1].group_ip, "224.81.128.11");
    EXPECT_EQ(cfgList[1].group_port, 8081);
    EXPECT_EQ(cfgList[1].ticker.ticker_type, "RandomDelayTicker");
    EXPECT_EQ(cfgList[1].ticker.packet_loss, 0.0);
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