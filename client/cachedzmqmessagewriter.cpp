#include "cachedzmqmessagewriter.h"
#define WRITE_QUEUE 20

#include "systemstatus.pb.h"

#include <iostream>
#include <fstream>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

CachedZMQMessageWriter::CachedZMQMessageWriter(){
    count = 0;
    writeCount = 0;
}   

CachedZMQMessageWriter::~CachedZMQMessageWriter(){

}

bool CachedZMQMessageWriter::push_message(google::protobuf::MessageLite* message){
    writeQueue_.push(message);
    count++;
    //Check size of Queue
    if(writeQueue_.size() >= WRITE_QUEUE){
        //Write stuff bruh
        std::cout << "Writing Messages" << std::endl;
        writeQueue();
    }

    //Try Read
    return true;
}

bool CachedZMQMessageWriter::terminate(){

    //read in all messages
    //writeQueue();
    std::cout << "TERMINATING: " << count << " write Count: " << writeCount << std::endl;
    std::queue<google::protobuf::MessageLite*> messages = readFromFile();

    while(!messages.empty()){
        if(!ZMQMessageWriter::push_message(messages.front())){
            std::cerr << "FAILED:!" << std::endl;
        }
        messages.pop();
    }
    while(!writeQueue_.empty()){
        if(!ZMQMessageWriter::push_message(writeQueue_.front())){
            std::cerr << "FAILED:!" << std::endl;
        }
        writeQueue_.pop();
    }
    std::cout << "FINISHED BRUH" << std::endl;
    return true;
}

bool CachedZMQMessageWriter::writeQueue(){
    std::cout << "writeQueue" << std::endl;
    //open a fstream
    std::fstream file("test.out", std::ios::out | std::ios::app | std::ios::binary);
    if (!file){
        std::cerr << "Failed to open file!" << std::endl;
        return false;
    }

    //Make an output stream
    ::google::protobuf::io::ZeroCopyOutputStream *raw_out = new ::google::protobuf::io::OstreamOutputStream(&file);

    //Write the messages bro!
    while(!writeQueue_.empty()){
        google::protobuf::MessageLite* message = writeQueue_.front();

        if(writeDelimitedTo(*message, raw_out)){
            writeCount++;
            
            writeQueue_.pop();
        }else{
            std::cout << "FAILED WRITING?!" << std::endl;
            return false;
        }
        //Clean up memory!
        delete message;
    }
    delete raw_out;
    file.close();
    return true;
}

std::queue<google::protobuf::MessageLite*> CachedZMQMessageWriter::readFromFile(){
    std::queue<google::protobuf::MessageLite*> queue;

    //open a fstream
    std::fstream file("test.out", std::ios::in | std::ios::binary);
    if (!file){
        std::cerr << "Failed to open file!" << std::endl;
        return queue;
    }

    //Make an output stream
    ::google::protobuf::io::ZeroCopyInputStream *raw_in = new ::google::protobuf::io::IstreamInputStream(&file);
    while(true){
        google::protobuf::MessageLite* message = new SystemStatus();
        
        if(readDelimitedFrom(raw_in, message)){
            queue.push(message);
        }else{
            //Free the memory
            delete message;
            
            if (file.eof()){
                break;
            }else{
                std::cout << " LOST A MESSAGE!" << std::endl;
                continue;
            }
        }
    }

    std::cout << "Got # " << queue.size() << " Messages!" << std::endl;
    delete raw_in;
    file.close();
    return queue;
}



bool CachedZMQMessageWriter::writeDelimitedTo(
    const google::protobuf::MessageLite& message,
    google::protobuf::io::ZeroCopyOutputStream* rawOutput) {
  // We create a new coded stream for each message.  Don't worry, this is fast.
  google::protobuf::io::CodedOutputStream output(rawOutput);

  // Write the size.
  const int size = message.ByteSize();
  output.WriteVarint32(size);

  uint8_t* buffer = output.GetDirectBufferForNBytesAndAdvance(size);
  if (buffer != NULL) {
    // Optimization:  The message fits in one buffer, so use the faster
    // direct-to-array serialization path.
    message.SerializeWithCachedSizesToArray(buffer);
  } else {
    // Slightly-slower path when the message is multiple buffers.
    message.SerializeWithCachedSizes(&output);
    if (output.HadError()) return false;
  }

  return true;
}


bool CachedZMQMessageWriter::readDelimitedFrom(
    google::protobuf::io::ZeroCopyInputStream* rawInput,
    google::protobuf::MessageLite* message) {
  // We create a new coded stream for each message.  Don't worry, this is fast,
  // and it makes sure the 64MB total size limit is imposed per-message rather
  // than on the whole stream.  (See the CodedInputStream interface for more
  // info on this limit.)
  google::protobuf::io::CodedInputStream input(rawInput);

  // Read the size.
  uint32_t size;
  if (!input.ReadVarint32(&size)) return false;

  // Tell the stream not to read beyond that size.
  google::protobuf::io::CodedInputStream::Limit limit =
      input.PushLimit(size);

  // Parse the message.
  if (!message->MergeFromCodedStream(&input)) return false;
  if (!input.ConsumedEntireMessage()) return false;

  // Release the limit.
  input.PopLimit(limit);

  return true;
}