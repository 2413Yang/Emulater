#include <memory>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <thread>
#include <csignal>
#include <vector>
#include "Factory.h"
#include "MessageFormatter.h"
#include "MulticastSender.h"

static bool readFileToBuffer(const std::string &filename, std::vector<uint8_t> &buffer);
static void cfgHandler(const EmulatorCfg* emu_cfg);
std::vector<std::shared_ptr<Service>> service_list;

static void sigint_handler(int);

int main(int argc, char *argv[]){
    signal(SIGINT,sigint_handler);
    if (argc < 2) {
      std::cerr << "Usage: " << argv[0] << "<config_file>" << std::endl;
      return EXIT_FAILURE;
    }
    std::string config_file = argv[1];
    std::shared_ptr<Config> config = createEmulaterCfg(YAML::LoadFile(config_file));
    std::vector<EmulatorCfg> cfg_list = config->load();
    std::vector<std::thread> threadlist(cfg_list.size());
    int cfg_num = cfg_list.size();
    for (int i = 0; i < cfg_num; i++){
      threadlist[i] = std::thread(cfgHandler,&cfg_list[i]);
    }
    for (int i = 0; i < cfg_num; i++){
      threadlist[i].join();
    }
}

static void cfgHandler(const EmulatorCfg* emu_cfg){
      std::string filename = emu_cfg->data_file;
      std::vector<uint8_t> buffer;
      if (!readFileToBuffer(filename, buffer)) {
          std::cout << emu_cfg->name <<"  readFileToBuffer failed." << std::endl;
          return;
      }
      
      std::unique_ptr<MessageFormatter> messageFormatter = nullptr;
      if (emu_cfg->mode == "HF" && emu_cfg->packet_size == getHFPacketSize()){
        messageFormatter = createHFMessageFormatter(std::move(buffer));
      }else if(emu_cfg->mode == "LF" && emu_cfg->packet_size == getLFPacketSize()){
        messageFormatter = createLFMessageFormatter(std::move(buffer));
      }else{
        std::cerr << "** Emulater Supper ** " << std::endl;
        std::cerr << "[mode]  [packet_size]" << std::endl;
        std::cerr << "  HF        2988 B" << std::endl;
        std::cerr << "  LF        3204 B" << std::endl;
        return;
      }
      if (messageFormatter == nullptr){
        std::cerr << emu_cfg->name <<"  Create MessageFormatter is failed." << std::endl;
        return;
      }

      auto curre_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
      auto start_time = curre_time + std::chrono::seconds(1);
      std::unique_ptr<Ticker> ticker = nullptr;
      if (emu_cfg->ticker.ticker_type == "SimpleTicker"){
        ticker = createTicker(start_time, emu_cfg->packets_per_second);
      }else if(emu_cfg->ticker.ticker_type == "RandomDelayTicker"){
        std::shared_ptr<Random> random = createRandom((1+ emu_cfg->ticker.packet_loss)/2);
        ticker = createPacketLossTicker(random,start_time,emu_cfg->packets_per_second);
      }else{
        std::cout << emu_cfg->name << "Ticker Support `SimpleTicker` or `RandomDelayTicker`" << std::endl;
        return;
      }

      auto service = createEmulaterService(std::move(ticker),std::move(messageFormatter),emu_cfg->group_ip,emu_cfg->group_port,emu_cfg->packet_size);
      service_list.push_back(service);
      service->start();  
}

static bool readFileToBuffer(const std::string &filename,std::vector<uint8_t> &buffer) {
  // 打开文件
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return false;
  }

  // 获取文件大小
  file.seekg(0, std::ios::end);
  std::streamsize fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  // 分配缓冲区大小并读取文件内容
  buffer.resize(fileSize);
  if (!file.read((char *)buffer.data(), fileSize)) {
    std::cerr << "Failed to read file: " << filename << std::endl;
    return false;
  }

  // 关闭文件并返回成功
  file.close();
  return true;
}

void sigint_handler(int){
  std::cout<< "SIGINT signal is triggered." << std::endl;
  for(auto& item : service_list){
    item->stop();
  }
}