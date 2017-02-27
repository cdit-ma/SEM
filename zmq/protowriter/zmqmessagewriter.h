#ifndef ZMQPROTOWRITER_ZMQMESSAGEWRITER_H
#define ZMQPROTOWRITER_ZMQMESSAGEWRITER_H

#include <mutex>
#include <google/protobuf/message_lite.h>


namespace zmq{
	class socket_t;
	class context_t;
}
class ZMQMessageWriter{
    public:
        ZMQMessageWriter();
        virtual ~ZMQMessageWriter();

        bool BindPublisherSocket(std::string endpoint);
        virtual void PushMessage(google::protobuf::MessageLite* message);
        virtual void Terminate();

    protected:
        void PushString(std::string* message_type, std::string* message);
        
    private:
        zmq::socket_t* socket_;
        zmq::context_t* context_;
        std::mutex mutex_;
};

#endif //ZMQPROTOWRITER_ZMQMESSAGEWRITER_H