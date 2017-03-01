#ifndef RE_COMMON_ZMQ_MONITOR_H
#define RE_COMMON_ZMQ_MONITOR_H
#include <functional>
#include <thread>
#include <zmq.hpp>

namespace zmq{
    class Monitor: public zmq::monitor_t{
        public:
            Monitor();
            ~Monitor();
            void MonitorSocket(zmq::socket_t* socket, std::string address, int event_type);
            void RegisterEventCallback(std::function<void(int, std::string)> fn);
        private:
            void on_event(const zmq_event_t &event, const char* addr);

            void on_event_connected(const zmq_event_t &event, const char* addr);
            void on_event_connect_delayed(const zmq_event_t &event, const char* addr);
            void on_event_connect_retried(const zmq_event_t &event, const char* addr);
            void on_event_listening(const zmq_event_t &event, const char* addr);
            void on_event_bind_failed(const zmq_event_t &event, const char* addr);
            void on_event_accepted(const zmq_event_t &event, const char* addr);
            void on_event_accept_failed(const zmq_event_t &event, const char* addr);
            void on_event_closed(const zmq_event_t &event, const char* addr);
            void on_event_close_failed(const zmq_event_t &event, const char* addr);
            void on_event_disconnected(const zmq_event_t &event, const char* addr);
            void on_event_handshake_failed(const zmq_event_t &event, const char* addr);
            void on_event_handshake_succeed(const zmq_event_t &event, const char* addr);
            void on_event_unknown(const zmq_event_t &event, const char* addr);

            std::function<void(int, std::string)> callback_;
            std::thread* monitor_thread_ = 0;
    };
}
#endif //RE_COMMON_ZMQ_MONITOR_H