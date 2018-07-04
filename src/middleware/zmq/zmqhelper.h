#ifndef ZMQ_ZMQHELPER_H
#define ZMQ_ZMQHELPER_H

#include <string>
#include <mutex>
#include <memory>

//Include ZMQ Headers
#include <zmq.hpp>

namespace zmq{
    class ZmqHelper{
        public:
            //Static getter functions
            static ZmqHelper& get_zmq_helper();
        public:
            std::unique_ptr<zmq::socket_t> get_publisher_socket();
            std::unique_ptr<zmq::socket_t> get_subscriber_socket();

            std::unique_ptr<zmq::socket_t> get_request_socket();
            std::unique_ptr<zmq::socket_t> get_reply_socket();

            int poll_socket(zmq::socket_t& socket, std::chrono::milliseconds timeout);
        private:
            zmq::context_t context_;
    };
};

#endif //ZMQ_ZMQHELPER_H

