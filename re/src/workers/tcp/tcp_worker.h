//
// Created by cdit-ma on 12/03/19.
//

#ifndef PROJECT_TCPWORKER_H
#define PROJECT_TCPWORKER_H

#include "worker.h"
#include <functional>

class TcpWorkerImpl;
class Tcp_Worker : public Worker {
    public:
    Tcp_Worker(const BehaviourContainer& container,
              const std::string& inst_name);
    ~Tcp_Worker() final;

    bool Bind();
    bool Connect();
    void Disconnect();

    bool SetTcpCallback(
        const std::function<void(const std::vector<char>&)>& callback_function);

    int Send(std::vector<char> data);
    int Receive(std::vector<char>& out);

    const std::string& get_version() const override;

    void HandleTerminate() override;
    void HandleConfigure() override;

    private:
    std::shared_ptr<TcpWorkerImpl> impl_;
    std::shared_ptr<Attribute> ip_address_;
    std::shared_ptr<Attribute> port_;
    std::shared_ptr<Attribute> delimiter_;
};

#endif // PROJECT_TCPWORKER_H
