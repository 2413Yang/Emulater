#include <gtest/gtest.h>
#include "EmulaterConfig.cpp"

TEST(EmulaterConfigTest, LoadFunction){
    std::string file_path = "../src/config/config.yaml";
    auto emulaterCfg = createEmulaterCfg(file_path);
    auto emuList = emulaterCfg->load();
    for(auto item = emuList.begin(); item != emuList.end(); item++){
        std::cout << "Emulator Name: " << item->name << std::endl;
        std::cout << "Packets per Second: " << item->packets_per_second << std::endl;
        std::cout << "Packet Size: " << item->packet_size << std::endl;
        std::cout << "Mode: " << item->mode << std::endl;
        std::cout << "Packet Loss: " << item->packet_loss << std::endl;
        std::cout << "Source File: " << item->source_file << std::endl;
        std::cout << "ip: " << item->ip << std::endl;
        std::cout << "port: " << item->port << std::endl;
        std::cout << std::endl;
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