#ifndef ZMQPROTOWRITER_CACHEDZMQMESSAGEWRITER_H
#define ZMQPROTOWRITER_CACHEDZMQMESSAGEWRITER_H

#include "zmqmessagewriter.h"

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

struct Message_Struct{
    Message_Struct(){
        type = new std::string();
        data = new std::string();
    };
    ~Message_Struct(){
        delete type;
        delete data;
    };
    std::string* type;
    std::string* data;
};

class CachedZMQMessageWriter : public ZMQMessageWriter{
    public:
        CachedZMQMessageWriter(int cache_count = 50);
        ~CachedZMQMessageWriter();
        
        void PushMessage(google::protobuf::MessageLite* message);
    private:
        

        void Terminate();
        void WriteQueue();

        std::queue<Message_Struct*> ReadMessagesFromFile(std::string file_path);

        bool WriteDelimitedTo(google::protobuf::MessageLite* message, google::protobuf::io::ZeroCopyOutputStream* raw_output);
        bool ReadDelimitedToStr(google::protobuf::io::ZeroCopyInputStream* raw_input, std::string* type, std::string* message);

        std::string temp_file_path_;

        std::thread* writer_thread_ = 0;


        int log_count_ = 0;
        int cache_count_ = 0;
        
        std::queue<google::protobuf::MessageLite*> write_queue_;
        
        std::condition_variable queue_lock_condition_;
        std::mutex queue_mutex_;
        bool writer_terminate_ = false;
};

#endif //ZMQPROTOWRITER_CACHEDZMQMESSAGEWRITER_H