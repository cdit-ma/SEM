#ifndef WORKERS_DIS_DISWORKER_IMPL_H
#define WORKERS_DIS_DISWORKER_IMPL_H

#include <string>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <future>
#include <queue>

#define DIS_VERSION 5

// DIS uses dynamic exception specifications in their headers
// This macro replaces any instance of throw(<exception_type>) in the wrapped header with throw()
// Necessary for cxx_std_17 compliance
#if __cplusplus >= 201703L
#define throw(...) throw()
#include <KDIS/Extras/PDU_Factory.h>
#include <KDIS/Network/Connection.h>
#include <KDIS/Network/ConnectionSubscriber.h>
#undef throw
#else
#include <KDIS/Extras/PDU_Factory.h>
#include <KDIS/Network/Connection.h>
#include <KDIS/Network/ConnectionSubscriber.h>
#endif //__cplusplus > 201703L

class Dis_Worker_Impl{
    public:
        Dis_Worker_Impl() = default;
        ~Dis_Worker_Impl();
        void Connect(const std::string& ip_address, const int port);
        void SetPduCallback(std::function<void (const KDIS::PDU::Header &)> func);
        std::string PDU2String(const KDIS::PDU::Header& header);
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
