#include "MulticastSender.h"
#include <yaml-cpp/yaml.h>
#include <iostream>

class EmulaterConfig : public Config{
public:  
    EmulaterConfig(YAML::Node emulators) : emulators_(emulators){}
    ~ EmulaterConfig() = default;  
  
    virtual  const std::vector<EmulatorCfg>& load() const override{
        try {
            // 清空之前的配置信息
            cfg_list_.clear();

            // 遍历 Emulators 列表
            for (const auto& emulator : emulators_["Emulators"]) {
                EmulatorCfg emu;
                emu.name = emulator["name"].as<std::string>();
                emu.mode = emulator["mode"].as<std::string>();
                emu.packets_per_second = emulator["packets_per_second"].as<int>();
                emu.packet_size = emulator["packet_size"].as<int>();
                emu.data_file = emulator["data_file"].as<std::string>();
                emu.group_ip = emulator["group_ip"].as<std::string>();
                emu.group_port = emulator["group_port"].as<int>();
                // 解析 Ticker 信息
                const auto& ticker_node = emulator["ticker"];
                emu.ticker.ticker_type = ticker_node["ticker_type"].as<std::string>();
                if (emu.ticker.ticker_type != "SimpleTicker") {
                    emu.ticker.packet_loss = ticker_node["packet_loss"].as<double>();
                }else{
                    if (ticker_node["packet_loss"]){
                        std::cerr << "SimpleTicker not support packet loss." << std::endl;
                        emu.ticker.packet_loss = ticker_node["packet_loss"].as<double>();
                    }
                }
                // 将解析得到的 EmulatorCfg 添加到配置列表中
                cfg_list_.push_back(emu);
            }
        } catch (const YAML::Exception& e) {
            throw std::out_of_range ("Error loading or parsing YAML file: " +std::string(e.what()));
        }
        
        return cfg_list_;
  }
private:
    YAML::Node emulators_;
    mutable std::vector<EmulatorCfg> cfg_list_;
};

std::shared_ptr<Config> createEmulaterCfg(const YAML::Node emulators){
    return  std::shared_ptr<Config>(new EmulaterConfig(emulators));
}