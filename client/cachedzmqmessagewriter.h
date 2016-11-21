#ifndef CACHEDZMQMESSAGEWRITER_H
#define CACHEDZMQMESSAGEWRITER_H
#include "zmqmessagewriter.h"

#include <queue>


class CachedZMQMessageWriter : public ZMQMessageWriter{
    public:
        CachedZMQMessageWriter();
        ~CachedZMQMessageWriter();

        bool push_message(google::protobuf::MessageLite* message);
        bool terminate();
    private:
        bool writeQueue();
        std::queue<google::protobuf::MessageLite*> readFromFile();


        bool writeDelimitedTo(const google::protobuf::MessageLite& message, google::protobuf::io::ZeroCopyOutputStream* rawOutput);
        bool readDelimitedFrom(google::protobuf::io::ZeroCopyInputStream* rawInput, google::protobuf::MessageLite* message);

        int count;
        int writeCount;
        std::queue<google::protobuf::MessageLite*> writeQueue_;        
};

#endif //CACHEDZMQMESSAGEWRITER_H