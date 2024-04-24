#include "Factory.h"
#include <memory>
#include <arpa/inet.h>
#include <thread>

class EmulaterService : public Service{
public:
    EmulaterService(std::unique_ptr<Ticker>&& ticker, std::unique_ptr<MessageFormatter>&& message_formasster, const int sockfd, const struct sockaddr_in addr, const int packet_size)
     : m_ticker_(std::move(ticker)),m_message_formatter_(std::move(message_formasster)),sockfd_(sockfd),addr_(addr),packet_size_(packet_size){}
    ~EmulaterService(){}
    virtual void serve() override{
        std::thread run(&EmulaterService::handel,this);
        run.detach();
    }
    virtual void stop() override{

    }
private:
    void handel(){
        while (true) {
            uint8_t data[packet_size_];
            auto result = m_ticker_->wait();
            m_message_formatter_->format(data, result.first, result.second);
            if (sendto(sockfd_, data, packet_size_, 0, (struct sockaddr *)&addr_, sizeof(addr_)) == -1) {
                fprintf(stderr, "[Disordered] sendto error. time: %ld packet: %d\n", result.first, result.second);
            }
        }
    }
private:
    std::unique_ptr<Ticker> m_ticker_;
    std::unique_ptr<MessageFormatter> m_message_formatter_;
    int sockfd_;
    struct sockaddr_in addr_;
    int packet_size_;
};

std::unique_ptr<Service> createEmulaterService(std::unique_ptr<Ticker> ticker, std::unique_ptr<MessageFormatter> message_formasster, const int sockfd, const struct sockaddr_in addr, const int packet_size){
    return std::make_unique<EmulaterService>(std::move(ticker),std::move(message_formasster),sockfd,addr,packet_size);
}