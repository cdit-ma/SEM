#ifndef RE_COMMON_ZMQ_CACHEDPROTOWRITER_H
#define RE_COMMON_ZMQ_CACHEDPROTOWRITER_H

#include "protowriter.h"

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

struct Message_Struct{
    Message_Struct(){
        topic = new std::string();
        type = new std::string();
        data = new std::string();
    };
    ~Message_Struct(){
        delete topic;
        delete type;
        delete data;
    };
    std::string* topic;
    std::string* type;
    std::string* data;
};

namespace zmq{
    class CachedProtoWriter : public zmq::ProtoWriter{
        public:
            CachedProtoWriter(int cache_count = 50);
            ~CachedProtoWriter();
            
            void PushMessage(std::string topic, google::protobuf::MessageLite* message);
        private:
            

            void Terminate();
            void WriteQueue();

            std::queue<Message_Struct*> ReadMessagesFromFile(std::string file_path);

            bool WriteDelimitedTo(std::string topic, google::protobuf::MessageLite* message, google::protobuf::io::ZeroCopyOutputStream* raw_output);
            bool ReadDelimitedToStr(google::protobuf::io::ZeroCopyInputStream* raw_input, std::string* topic, std::string* type, std::string* message);

            std::string temp_file_path_;

            std::thread* writer_thread_ = 0;


            int log_count_ = 0;
            int cache_count_ = 0;
            
            std::queue<google::protobuf::MessageLite*> write_queue_;
            
            std::condition_variable queue_lock_condition_;
            std::mutex queue_mutex_;
            bool writer_terminate_ = false;
    };
};

#endif //RE_COMMON_ZMQ_CACHEDPROTOWRITER_H