#include "MulticastSender.h"
#include <yaml-cpp/yaml.h>

class EmulaterConfig : public Config{
public:  
    EmulaterConfig(const std::string& load_file) : load_file_(load_file){}
    ~ EmulaterConfig() = default;  
  
    virtual  const std::vector<EmulatorCfg>& load() const override{
        try {
            // 从文件中加载 YAML 文档
            YAML::Node emulators = YAML::LoadFile(load_file_);

            // 清空之前的配置信息
            cfg_list_.clear();

            // 遍历 Emulators 列表
            for (const auto& emulator : emulators["Emulators"]) {
                EmulatorCfg emu;
                emu.name = emulator["name"].as<std::string>();
                emu.packets_per_second = emulator["packets_per_second"].as<int>();
                emu.packet_size = emulator["packet_size"].as<int>();
                emu.mode = emulator["mode"].as<std::string>();
                emu.packet_loss = emulator["packet_loss"].as<double>();
                emu.source_file = emulator["source_file"].as<std::string>();
                emu.ip = emulator["ip"].as<std::string>();
                emu.ip = emulator["port"].as<int>();
                // 将解析得到的 EmulatorCfg 添加到配置列表中
                cfg_list_.push_back(emu);
            }
        } catch (const YAML::Exception& e) {
            // 加载或解析失败，可以根据实际情况处理异常
            throw std::out_of_range ("Error loading or parsing YAML file: " +std::string(e.what()));
        }
        // 返回解析得到的配置列表
        return cfg_list_;
  }
private:
    std::string load_file_;
    mutable std::vector<EmulatorCfg> cfg_list_;
};

std::shared_ptr<Config> createEmulaterCfg(const std::string file_path){
    return  std::shared_ptr<Config>(new EmulaterConfig(file_path));
}