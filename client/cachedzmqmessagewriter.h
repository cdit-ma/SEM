#ifndef CACHEDZMQMESSAGEWRITER_H
#define CACHEDZMQMESSAGEWRITER_H
#include "zmqmessagewriter.h"

#include <queue>


class CachedZMQMessageWriter : public ZMQMessageWriter{
    public:
        CachedZMQMessageWriter();
        ~CachedZMQMessageWriter();

        bool PushMessage(google::protobuf::MessageLite* message);
        bool Terminate();
    private:
        bool WriteQueue();
        std::queue<google::protobuf::MessageLite*> ReadFromFile();


        bool WriteDelimitedTo(const google::protobuf::MessageLite& message, google::protobuf::io::ZeroCopyOutputStream* rawOutput);
        bool ReadDelimitedFrom(google::protobuf::io::ZeroCopyInputStream* rawInput, google::protobuf::MessageLite* message);

        int count_ = 0;
        int write_count_ = 0;
        std::queue<google::protobuf::MessageLite*> write_queue_;        
};

#endif //CACHEDZMQMESSAGEWRITER_H