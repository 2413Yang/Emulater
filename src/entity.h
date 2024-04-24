#ifndef ENTITY_H
#define ENTITY_H
#include <string>
#include <vector>

struct EmulatorCfg {
    std::string name;
    int packets_per_second;
    int packet_size;
    std::string mode;
    double packet_loss;
    std::string source_file;
    std::string ip;
    int port;
};

#endif //ENTITY_H