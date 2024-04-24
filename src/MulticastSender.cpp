#include <memory>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include "Factory.h"
#include "MessageFormatter.h"

static std::pair<int, struct sockaddr_in> initializeSocket(const std::string &multicastGroup, int port);
static bool readFileToBuffer(const std::string &filename, std::vector<uint8_t> &buffer);

int main(int argc, char *argv[]){
    if (argc < 2) {
      std::cerr << "Usage: " << argv[0] << "<config_file_path>" << std::endl;
      return EXIT_FAILURE;
    }
    std::string config_file = argv[1];
    std::shared_ptr<Config> config = createEmulaterCfg(config_file);
    std::vector<EmulatorCfg> cfg_list = config->load();
    for (auto item = cfg_list.begin(); item != cfg_list.end(); item++){
      std::cout << item->name << std::endl;
      auto result = initializeSocket(item->ip,item->port);
      int sockfd = result.first;
      struct sockaddr_in addr = result.second;
      std::vector<uint8_t> buffer;
      if (!readFileToBuffer(item->source_file, buffer)) {
          return 1;
      }
      std::unique_ptr<MessageFormatter> messageFormatter = nullptr;
      if (item->packet_size == getHFPacketSize()){
          messageFormatter = createHFMessageFormatter(std::move(buffer));
      }else if(item->packet_size == getLFPacketSize()){
          messageFormatter = createLFMessageFormatter(std::move(buffer));
      }
      if (messageFormatter == nullptr) {
          std::cerr << "Body buffer capacity is too low." << std::endl;
          return 1;
      }
      auto curre_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
      auto start_time = curre_time + std::chrono::seconds(1);
      std::cout << "ticker" << std::endl;
      auto ticker = createTicker(start_time, item->packets_per_second);
      std::cout << "service" << std::endl;
      auto service = createEmulaterService(std::move(ticker),std::move(messageFormatter),sockfd,addr,item->packet_size);
      service->serve();
      std::cout << "end" << std::endl;
    }
}

// 初始化socket
static std::pair<int, struct sockaddr_in> initializeSocket(const std::string &multicastGroup, int port) {
  // 创建 UDP 套接字
  int sockfd = -1;
  struct sockaddr_in addr;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    std::cerr << "Failed to create socket. " << std::endl;
    return {-1, addr};
  }
  // 设置套接字选项
  int ttl = 64;
  if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) == -1) {
    std::cerr << "Failed to set socket options. " << std::endl;
    return {-1, addr};
  }
  // 设置目标地址
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(multicastGroup.c_str());
  addr.sin_port = htons(port);

  return {sockfd, addr};
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