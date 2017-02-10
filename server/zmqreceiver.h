#ifndef ZMQRECEIVER_H
#define ZMQRECEIVER_H

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <functional>

#include "zmq.hpp"
#include "logdatabase.h"

#include <google/protobuf/message_lite.h>
#include <google/protobuf/message.h>

class ZMQReceiver{
    public:
        ZMQReceiver(std::vector<std::string> addrs, std::string port, std::string file);
        ~ZMQReceiver();

        void TerminateReceiver();
        void SetProtoHandler(LogDatabase* log_database);
        void Start();
        
        void RegisterNewProto(google::protobuf::MessageLite* ml);
    private:

        google::protobuf::MessageLite* ConstructMessage(std::string type, std::string data);
        void RecieverThread();
        void ProtoConvertThread();
        
        std::vector<std::string> addresses_;
        std::string port_;

        std::thread* reciever_thread_;
        std::thread* proto_convert_thread_;

        zmq::context_t *context_;
        zmq::socket_t *term_socket_;

        LogDatabase* log_database_;

        std::map<std::string, std::function< google::protobuf::MessageLite* ()> > proto_lookup_;
        std::queue<google::protobuf::MessageLite*> types_;

        std::condition_variable queue_lock_condition_;
        std::mutex queue_mutex_;
        std::queue<std::pair<std::string, std::string> > rx_message_queue_; 
        
        bool terminate_reciever_ = false;
        bool terminate_proto_convert_thread_ = false;
};

#endif //ZMQRECEIVER_H