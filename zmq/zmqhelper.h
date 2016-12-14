#ifndef ZMQHELPER_H
#define ZMQHELPER_H

#include <string>
#include "zmq.hpp"

namespace zmq{
    class ZmqHelper{
        public:
            static ZmqHelper* get_zmq_helper();

            zmq::context_t* get_context();
            zmq::socket_t* get_publisher_socket();
            zmq::socket_t* get_subscriber_socket();
            
        private:
            static ZmqHelper* singleton_;
            zmq::context_t* context_;
    };
};

#endif //ZMQHELPER_H