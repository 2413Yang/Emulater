#ifndef ENTITY_H
#define ENTITY_H
#include <string>

struct TickerInfo {
    std::string ticker_type;
    double packet_loss;
};

struct EmulatorCfg {
    std::string name;
    std::string mode;
    int packets_per_second;
    int packet_size;
    std::string data_file;
    std::string group_ip;
    int group_port;
    TickerInfo ticker;
};

#endif //ENTITY_H