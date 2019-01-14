#ifndef WORKERS_DIS_DISWORKER_IMPL_H
#define WORKERS_DIS_DISWORKER_IMPL_H

#include <string>

#include <KDIS/Extras/PDU_Factory.h>
#include <KDIS/Network/Connection.h>
#include <KDIS/Network/ConnectionSubscriber.h>

class Dis_Worker_Impl{
    public:
        Dis_Worker_Impl();

        void Connect(const std::string& ip_address, const int port);
        void Disconnect();
    private:
        std::unique_ptr<KDIS::NETWORK::Connection> connection_;
};

class Dis_EventSubscriber : public KDIS::NETWORK::ConnectionSubscriber{

};

#endif //WORKERS_DIS_DISWORKER_IMPL_H
