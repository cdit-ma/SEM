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

        void Start();
        
        void Connect(std::string address, std::string topic_filter);
        void RegisterNewProto(const google::protobuf::MessageLite &ml, std::function<void(google::protobuf::MessageLite*)> fn);
    private:
        google::protobuf::MessageLite* ConstructMessage(std::string type, std::string data);
        void Connect_(std::string address);
        void Filter_(std::string topic_filter);
        void RecieverThread();
        void ProtoConvertThread();

        zmq::socket_t* socket_ = 0;

        int batch_size_;
        
        std::vector<std::pair<std::string, std::string> > addresses_;

        std::thread* reciever_thread_ = 0;
        std::thread* proto_convert_thread_ = 0;

        zmq::context_t *context_ = 0;
        //zmq::socket_t *term_socket_ = 0;

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