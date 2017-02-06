#ifndef CACHEDZMQMESSAGEWRITER_H
#define CACHEDZMQMESSAGEWRITER_H
#include "zmqmessagewriter.h"

#include <queue>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>

class CachedZMQMessageWriter : public ZMQMessageWriter{
    public:
        CachedZMQMessageWriter();
        ~CachedZMQMessageWriter();

        bool PushMessage(google::protobuf::MessageLite* message);
        bool Terminate();
    private:
        void WriteQueue();
        std::queue<google::protobuf::MessageLite*> ReadFromFile();


        bool WriteDelimitedTo(const google::protobuf::MessageLite& message, google::protobuf::io::ZeroCopyOutputStream* rawOutput);
        bool ReadDelimitedFrom(google::protobuf::io::ZeroCopyInputStream* rawInput, google::protobuf::MessageLite* message);

        std::string temp_file_path_;

        std::thread* writer_thread_;

        std::map<std::string, std::string> temp_file_paths_;

        int count_ = 0;
        int write_count_ = 0;
        std::queue<google::protobuf::MessageLite*> write_queue_;
        std::condition_variable queue_lock_condition_;
        std::mutex queue_mutex_;

        bool writer_terminate_ = false;

};

#endif //CACHEDZMQMESSAGEWRITER_H