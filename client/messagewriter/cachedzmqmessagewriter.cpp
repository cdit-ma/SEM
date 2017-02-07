#include "cachedzmqmessagewriter.h"
#define WRITE_QUEUE_MAX 50

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

    writer_thread_ = new std::thread(&CachedZMQMessageWriter::WriteQueue, this);
}   

CachedZMQMessageWriter::~CachedZMQMessageWriter(){
    //Call terminate to read then send all of the messages
    Terminate();
}

bool CachedZMQMessageWriter::PushMessage(google::protobuf::MessageLite* message){
    std::unique_lock<std::mutex> lock(queue_mutex_);
    write_queue_.push(message);
    count_++;
    if(write_queue_.size() >= WRITE_QUEUE_MAX){
        queue_lock_condition_.notify_all();
    }
    return true;
}

bool CachedZMQMessageWriter::Terminate(){

    {
        //Gain the lock so we can notify and set our terminate flag.
        std::unique_lock<std::mutex> lock(queue_mutex_);
        writer_terminate_ = true;
        queue_lock_condition_.notify_all();
    }

    writer_thread_->join();

    std::cout << "TERMINATING: " << count_ << " write Count: " << write_count_ << std::endl;
    
    int message_count = 0;
    //Send messages in temp file
    
    auto messages = ReadFromFile();
    int count = 0;

    while(!messages.empty()){
        auto s = messages.front();
        messages.pop();
        if(s){
            if(ZMQMessageWriter::PushString(s)){
                message_count++;
            }else{
                std::cerr << "FAILED:!" << std::endl;
            }
            //Free Memory after writing
            delete s;
        }
    }

    //Send messages in write queue
    while(!write_queue_.empty()){
        auto m = write_queue_.front();
        write_queue_.pop();
        if(m){
            if(ZMQMessageWriter::PushMessage(m)){
                message_count ++;
            }else{
                std::cerr << "FAILED:!" << std::endl;
            }
        }
    }

    std::cout << "Written: " << message_count << "/" << count_ << std::endl;

    std::remove(temp_file_path_.c_str());

    return true;
}

void CachedZMQMessageWriter::WriteQueue(){
    while(!writer_terminate_){
        std::queue<google::protobuf::MessageLite*> replace_queue;

        {
            //Obtain lock for the queue
            std::unique_lock<std::mutex> lock(queue_mutex_);
            //Wait for notify
            queue_lock_condition_.wait(lock);
            
            if(writer_terminate_){
                break;
            }

            write_queue_.swap(replace_queue);
        }

        if(replace_queue.empty()){
            //Ignore empty stuffs
            continue;
        }

        //open a fstream
        //TODO: lookup file in temp file map.
        std::fstream file(temp_file_path_, std::ios::out | std::ios::app | std::ios::binary);
        if (!file){
            std::cerr << "Failed to open file!" << std::endl;
        }

        //Make an output stream
        ::google::protobuf::io::ZeroCopyOutputStream *raw_out = new ::google::protobuf::io::OstreamOutputStream(&file);

        //Write the messages bro!
        while(!replace_queue.empty()){
            google::protobuf::MessageLite* message = replace_queue.front();

            if(WriteDelimitedTo(*message, raw_out)){
                write_count_++;
                replace_queue.pop();
            }else{
                std::cout << "Error writing message to file!" << std::endl;
            }
            //Clean up memory!
            delete message;
        }
        delete raw_out;
        file.close();
    }

    std::cout << "Terminating cached writer thread" << std::endl;
}

std::queue<std::string*> CachedZMQMessageWriter::ReadFromFile(){
    std::queue<std::string*> queue;

    //open a fstream
    std::fstream file(temp_file_path_, std::ios::in | std::ios::binary);
    if (!file){
        std::cerr << "Failed to open file!" << std::endl;
        return queue;
    }

    //Make an output stream
    ::google::protobuf::io::ZeroCopyInputStream *raw_in = new ::google::protobuf::io::IstreamInputStream(&file);
    while(true){
        //Allocate a new string to store the read data
        std::string *message_str = new std::string();

        //Read the proto encoded string into our message and queue if succesful
        if(ReadDelimited2Str(raw_in, message_str)){
            queue.push(message_str);
        }else{
            //On Error, free memory and check for EOF
            delete message_str;
            
            if(file.eof()){
                std::cout << "EOF" << std::endl;
                break;
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

bool CachedZMQMessageWriter::ReadDelimited2Str(
    google::protobuf::io::ZeroCopyInputStream* rawInput, std::string* message) {
    
    google::protobuf::io::CodedInputStream input(rawInput);

    // Read the size.
    uint32_t size;
    if(!input.ReadVarint32(&size)){
        return false;
    }

    // Parse the message.
    if(!input.ReadString(message, size)){
        std::cout << "Stream Error @ " << input.CurrentPosition() << " Reading " << size << std::endl;
        std::cout << "ERROR!" << std::endl;
        return false;
    }
    return true;
}
