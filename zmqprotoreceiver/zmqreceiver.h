#ifndef ZMQRECEIVER_H
#define ZMQRECEIVER_H

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <functional>
#include <map>

namespace zmq{
	class socket_t;
	class context_t;
}


#include <google/protobuf/message_lite.h>
#include <google/protobuf/message.h>

class ZMQReceiver{
    public:
        ZMQReceiver(int batch_size = 20);
        ~ZMQReceiver();

        void TerminateReceiver();
        
        void Start();
        
        void Connect(std::string address);
        void RegisterNewProto(const google::protobuf::MessageLite &ml, std::function<void(google::protobuf::MessageLite*)> fn);
    private:

        zmq::socket_t* socket_ = 0;

        google::protobuf::MessageLite* ConstructMessage(std::string type, std::string data);
        void RecieverThread();
        void ProtoConvertThread();

        int batch_size_;
        
        std::vector<std::string> addresses_;

        std::thread* reciever_thread_;
        std::thread* proto_convert_thread_;

        zmq::context_t *context_ = 0;
        zmq::socket_t *term_socket_ = 0;

        std::map<std::string, std::function<void(google::protobuf::MessageLite*)> > callback_lookup_;
        std::map<std::string, std::function< google::protobuf::MessageLite* ()> > proto_lookup_;

        std::condition_variable queue_lock_condition_;
        std::mutex queue_mutex_;
        std::queue<std::pair<std::string, std::string> > rx_message_queue_; 

        std::mutex address_mutex_;
        
        bool terminate_reciever_ = false;
        bool terminate_proto_convert_thread_ = false;
};

#endif //ZMQRECEIVER_H