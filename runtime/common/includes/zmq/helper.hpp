#ifndef ZMQHELPER_H
#define ZMQHELPER_H

#include <string>
#include "zmq.hpp"

namespace zmq{
    class ZmqHelper{
        public:
            static ZmqHelper* get_zmq_helper(){
                static zmq::ZmqHelper* singleton_ = 0;
                
                if(singleton_ == 0){
                    singleton_ = new ZmqHelper();
                }
                return singleton_;
            }

            zmq::context_t* get_context(){
                if(context_ == 0){
                    context_ = new zmq::context_t(1); 
                }
                return context_;
            }
            zmq::socket_t* get_publisher_socket(){
                auto socket = new zmq::socket_t(*get_context(), ZMQ_PUB);
                return socket;
            }
            zmq::socket_t* get_subscriber_socket(){
                auto socket = new zmq::socket_t(*get_context(), ZMQ_SUB);
                socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
                return socket;
            }
            
        private:
            static ZmqHelper* singleton_;
            zmq::context_t* context_;
    };
};

#endif //ZMQHELPER_H

