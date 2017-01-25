#include "cachedzmqmessagewriter.h"
#define WRITE_QUEUE 50

#include "systemstatus.pb.h"

#include <iostream>
#include <fstream>
#include <cstdio>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include <boost/filesystem.hpp>

CachedZMQMessageWriter::CachedZMQMessageWriter() : ZMQMessageWriter(){
    auto temp = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    temp_file_path_ = temp.native();
    std::cout << temp_file_path_ << std::endl;
}   

CachedZMQMessageWriter::~CachedZMQMessageWriter(){
    //Call terminate to read then send all of the messages
    Terminate();
}

bool CachedZMQMessageWriter::PushMessage(google::protobuf::MessageLite* message){
    write_queue_.push(message);
    count_++;
    //Check size of Queue
    if(write_queue_.size() >= WRITE_QUEUE){
        //Write stuff bruh
        std::cout << "Writing Messages" << std::endl;
        WriteQueue();
    }

    //Try Read
    return true;
}

bool CachedZMQMessageWriter::Terminate(){

    //read in all messages
    std::cout << "TERMINATING: " << count_ << " write Count: " << write_count_ << std::endl;

    //Send messages in temp file
    std::queue<google::protobuf::MessageLite*> messages = ReadFromFile();
    int count = 0;
    while(!messages.empty()){
        if(!ZMQMessageWriter::PushMessage(messages.front())){
            std::cerr << "FAILED:!" << std::endl;
        }
        count++;
        messages.pop();
        
    }

    //Send messages in write queue
    while(!write_queue_.empty()){
        if(!ZMQMessageWriter::PushMessage(write_queue_.front())){
            std::cerr << "FAILED:!" << std::endl;
        }
        count++;
        write_queue_.pop();
    }
    std::cout << "Written: " << count << std::endl;

    std::remove(temp_file_path_.c_str());

    return true;
}

bool CachedZMQMessageWriter::WriteQueue(){
    //open a fstream
    std::fstream file(temp_file_path_, std::ios::out | std::ios::app | std::ios::binary);
    if (!file){
        std::cerr << "Failed to open file!" << std::endl;
        return false;
    }

    //Make an output stream
    ::google::protobuf::io::ZeroCopyOutputStream *raw_out = new ::google::protobuf::io::OstreamOutputStream(&file);

    //Write the messages bro!
    while(!write_queue_.empty()){
        google::protobuf::MessageLite* message = write_queue_.front();

        if(WriteDelimitedTo(*message, raw_out)){
            write_count_++;
            
            write_queue_.pop();
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

std::queue<google::protobuf::MessageLite*> CachedZMQMessageWriter::ReadFromFile(){
    std::queue<google::protobuf::MessageLite*> queue;

    //open a fstream
    std::fstream file(temp_file_path_, std::ios::in | std::ios::binary);
    if (!file){
        std::cerr << "Failed to open file!" << std::endl;
        return queue;
    }

    //Make an output stream
    ::google::protobuf::io::ZeroCopyInputStream *raw_in = new ::google::protobuf::io::IstreamInputStream(&file);
    while(true){
        google::protobuf::MessageLite* message = new SystemStatus();
        
        if(ReadDelimitedFrom(raw_in, message)){
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



bool CachedZMQMessageWriter::WriteDelimitedTo(
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


bool CachedZMQMessageWriter::ReadDelimitedFrom(
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