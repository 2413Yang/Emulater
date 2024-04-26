#include "Factory.h"
#include <memory>
#include <arpa/inet.h>
#include <thread>
#include <unistd.h>
#include <iostream>
#include <cstring>

class EmulaterService : public Service{
public:
    EmulaterService(std::unique_ptr<Ticker>&& ticker, std::unique_ptr<MessageFormatter>&& message_formasster, const std::string group_ip, const int group_port, const int packet_size)
     : m_ticker_(std::move(ticker)),m_message_formatter_(std::move(message_formasster)),group_ip_(group_ip),group_port_(group_port),packet_size_(packet_size), is_run_(true){}
    ~EmulaterService(){ close(sockfd_); }
    virtual void start() override{
        this->initudpsocket();
        this->handel();
    }
    virtual void stop() override{
        is_run_ = false;
    }
private:
    void handel(){
        while (is_run_) {
            uint8_t data[packet_size_];
            auto result = m_ticker_->wait();
            m_message_formatter_->format(data, result.first, result.second);
            if (sendto(sockfd_, data, packet_size_, 0, (struct sockaddr *)&addr_, sizeof(addr_)) == -1) {
                fprintf(stderr, "[Disordered] sendto error. time: %ld packet: %d\n", result.first, result.second);
            }
        }
    }

    void initudpsocket(){
        if ((sockfd_ = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            std::cerr << "Failed to create socket. " << std::endl;
            return;
        }
        // 设置套接字选项
        int ttl = 64;
        if (setsockopt(sockfd_, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) == -1) {
            std::cerr << "Failed to set socket options. " << std::endl;
            return;
        }
        // 设置目标地址
        memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_addr.s_addr = inet_addr(group_ip_.c_str());
        addr_.sin_port = htons(group_port_);
    }
private:
    std::unique_ptr<Ticker> m_ticker_;
    std::unique_ptr<MessageFormatter> m_message_formatter_;
    std::string group_ip_;
    int group_port_;
    int packet_size_;
    bool is_run_;
    int sockfd_;
    struct sockaddr_in addr_;
};

std::shared_ptr<Service> createEmulaterService(std::unique_ptr<Ticker> ticker, std::unique_ptr<MessageFormatter> message_formasster, const std::string group_ip, const int group_port, const int packet_size){
    return std::shared_ptr<EmulaterService>(new EmulaterService(std::move(ticker),std::move(message_formasster),group_ip,group_port,packet_size));
}