#ifndef WORKERS_DIS_DISWORKER_IMPL_H
#define WORKERS_DIS_DISWORKER_IMPL_H

#include <string>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <future>
#include <queue>

#include <KDIS/Extras/PDU_Factory.h>
#include <KDIS/Network/Connection.h>
#include <KDIS/Network/ConnectionSubscriber.h>

class Dis_Worker_Impl{
    public:
        Dis_Worker_Impl();
        ~Dis_Worker_Impl();
        void Connect(const std::string& ip_address, const int port);
        void SetPduCallback(std::function<void (const KDIS::PDU::Header &)> func);
        void Disconnect();
    private:
        void ProcessEvents(std::unique_ptr<KDIS::NETWORK::Connection> connection);
        void ProcessQueue();

        std::mutex future_mutex_;

        std::atomic_bool terminate_kdis_;
        std::atomic_bool terminate_queue_;
        std::future<void> kdis_future_;
        std::future<void> callback_future_;

        std::mutex queue_mutex_;
        std::condition_variable queue_condition_;
        std::queue< std::unique_ptr<KDIS::PDU::Header> > pdu_queue_;

        std::function<void (const KDIS::PDU::Header &)> callback_function_;
};

#endif //WORKERS_DIS_DISWORKER_IMPL_H
