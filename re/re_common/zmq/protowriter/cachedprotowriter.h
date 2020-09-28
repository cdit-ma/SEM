#ifndef RE_COMMON_ZMQ_CACHEDPROTOWRITER_H
#define RE_COMMON_ZMQ_CACHEDPROTOWRITER_H

#include "protowriter.h"

#include <list>
#include <future>
#include <mutex>
#include <condition_variable>

struct Message_Struct{
    std::string topic;
    std::string type;
    std::string data;
};

namespace zmq{
    class CachedProtoWriter : public zmq::ProtoWriter{
        public:
            CachedProtoWriter(int cache_count = 50);
            ~CachedProtoWriter();
            
            bool PushMessage(const std::string& topic, std::unique_ptr<google::protobuf::MessageLite> message) override;
            void Terminate() override;
        private:
            void WriteQueue();

            std::list<std::unique_ptr<Message_Struct> > ReadMessagesFromFile(const std::string& file_path);

            bool WriteDelimitedTo(const std::string& topic, const google::protobuf::MessageLite& message, google::protobuf::io::ZeroCopyOutputStream* raw_output);
            bool ReadDelimitedToStr(google::protobuf::io::ZeroCopyInputStream* raw_input, std::string& topic, std::string& type, std::string& message);

            std::mutex mutex_;

            std::string temp_file_path_;

            std::future<void> writer_future_;


            int log_count_ = 0;
            int written_to_disk_count = 0;
            int cache_count_ = 0;
            
            std::list<std::pair<std::string, std::unique_ptr<google::protobuf::MessageLite> > > write_queue_;
            
            std::mutex queue_mutex_;
            std::condition_variable queue_lock_condition_;
            //std::mutex ;
            bool writer_terminate_ = false;
            bool running = false;
    };
};

#endif //RE_COMMON_ZMQ_CACHEDPROTOWRITER_H