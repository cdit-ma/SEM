#ifndef RE_COMMON_ZMQ_MONITOR_H
#define RE_COMMON_ZMQ_MONITOR_H
#include <zmq.hpp>
#include <unordered_map>
#include <functional>
#include <future>
#include <atomic>

namespace zmq{
    class ProtoWriter;
    class Monitor: public zmq::monitor_t{
        typedef std::function<void(int, std::string)> EventCallbackFn;

        friend class ProtoWriter;
        public:
            Monitor(zmq::socket_t& socket);
            ~Monitor();
            void RegisterEventCallback(const uint8_t& event_type, EventCallbackFn fn);
        private:
            void MonitorThread(std::reference_wrapper<zmq::socket_t> socket, const int event_type);
            
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

            std::mutex callback_mutex_;
            std::unordered_map< uint8_t, EventCallbackFn > callbacks_;
            
            std::future<void> future_;
            std::atomic_bool abort_{false};
    };
}
#endif //RE_COMMON_ZMQ_MONITOR_H