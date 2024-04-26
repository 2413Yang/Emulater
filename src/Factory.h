#ifndef FACTORY_H
#define FACTORY_H

#include "MulticastSender.h"
#include <memory>
#include <vector>
#include <chrono>
#include <yaml-cpp/yaml.h>

std::shared_ptr<Config> createEmulaterCfg(const YAML::Node emulators);

std::shared_ptr<Random> createRandom(double cumulative_probability);
std::unique_ptr<Ticker> createTicker(std::chrono::seconds start_time, std::int64_t packets_per_second);
std::unique_ptr<Ticker> createPacketLossTicker(std::shared_ptr<Random> random, std::chrono::seconds start_time, int64_t packets_per_second);

std::unique_ptr<MessageFormatter> createHFMessageFormatter(std::vector<uint8_t>&& buffer);
std::unique_ptr<MessageFormatter> createLFMessageFormatter(std::vector<uint8_t>&& buffer);

std::shared_ptr<Service> createEmulaterService(std::unique_ptr<Ticker> ticker, std::unique_ptr<MessageFormatter> message_formasster, const std::string group_ip, const int group_port, const int packet_size);
#endif // FACTORY_H