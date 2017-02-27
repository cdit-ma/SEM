#include "cachedzmqmessagewriter.h"

#include <cstdio>
#include <iostream>
#include <fstream>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <boost/filesystem.hpp>

CachedZMQMessageWriter::CachedZMQMessageWriter(int cache_count) : ZMQMessageWriter(){
    cache_count_ = cache_count;
    //Get a temporary file location for our cached files
    auto temp = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    temp_file_path_ = temp.string();
    std::cout << "Temporary file path: " << temp_file_path_ << std::endl;

    //Start the writer thread
    writer_thread_ = new std::thread(&CachedZMQMessageWriter::WriteQueue, this);
}   

CachedZMQMessageWriter::~CachedZMQMessageWriter(){
    //Terminate
    Terminate();
}

//Takes ownership of message
void CachedZMQMessageWriter::PushMessage(google::protobuf::MessageLite* message){
    //Gain the lock
    std::unique_lock<std::mutex> lock(queue_mutex_);
    //Push the message onto the queue
    write_queue_.push(message);
    log_count_ ++;
    if(write_queue_.size() >= cache_count_){
        //Notify the writer_thread to flush the queue if we have hit our write limit
        queue_lock_condition_.notify_all();
    }
}

void CachedZMQMessageWriter::Terminate(){
    {
        //Gain the lock so we can notify and set our terminate flag.
        std::unique_lock<std::mutex> lock(queue_mutex_);
        writer_terminate_ = true;
        queue_lock_condition_.notify_all();
    }

    //Wait for the writer_thread to finish
    writer_thread_->join();

    int sent_count = 0;
    
    //Read the messages from the queue
    auto messages = ReadMessagesFromFile(temp_file_path_);

    //Send the messages serialized in the temp file
    while(!messages.empty()){
        auto s = messages.front();
        messages.pop();
        if(s){
            ZMQMessageWriter::PushString(s->type, s->data);
            sent_count ++;
            delete s;
        }
    }

    //Send the Messages still in the write queue
    while(!write_queue_.empty()){
        auto m = write_queue_.front();
        write_queue_.pop();
        if(m){
            ZMQMessageWriter::PushMessage(m);
            sent_count ++;
        }
    }

    std::cout << "Successfully Written: " << sent_count << "/" << log_count_ << std::endl;

    //Remove the temp file
    std::remove(temp_file_path_.c_str());
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
            //Swap the queue
            write_queue_.swap(replace_queue);
        }
        

        if(replace_queue.empty()){
            //Ignore empty queue
            continue;
        }

        //Open temp file for appending binary data
        std::fstream file(temp_file_path_, std::ios::out | std::ios::app | std::ios::binary);
        if(!file){
            std::cerr << "Failed to open temp file '" << temp_file_path_ << "' to write." << std::endl;
            break;
        }

        //Make an output stream using the file
        auto raw_output = new ::google::protobuf::io::OstreamOutputStream(&file);
        
        //Write all messages in queue
        while(!replace_queue.empty()){
            auto message = replace_queue.front();
            replace_queue.pop();

            if(!message || !WriteDelimitedTo(message, raw_output)){
                std::cerr << "Error writing message to temp file '" << temp_file_path_ << "'" << std::endl;
            }

            //Delete protobuf message
            delete message;
        }
        //Delete output stream
        delete raw_output;
        //Close file
        file.close();
    }
    std::cout << "Cached Writer Thread finished!" << std::endl;
}

std::queue<Message_Struct*> CachedZMQMessageWriter::ReadMessagesFromFile(std::string file_path){
    //Construct a return queue
    std::queue<Message_Struct*> queue;

    //Open temp file for reading binary data
    std::fstream file(file_path, std::ios::in | std::ios::binary);
    if(!file){
        std::cerr << "Failed to open file '" << file_path << "' to write." << std::endl;
        return queue;
    }

    //Make an output stream using the file
    auto raw_input = new ::google::protobuf::io::IstreamInputStream(&file);
    
    while(true){
        //Allocate a new string to store the read data
        Message_Struct *m = new Message_Struct();

        //Read the proto encoded string into our message and queue if successful
        if(ReadDelimitedToStr(raw_input, m->type, m->data)){
            queue.push(m);
        }else{
            //If we have an error, free memory
            delete m;
            
            //Check for end of file
            if(file.eof()){
                break;
            }
        }
    }

    std::cout << "Read # " << queue.size() << " Messages from '" << file_path << "'" << std::endl;
    
    //Delete input stream
    delete raw_input;
    //Close file
    file.close();
    return queue;
}

bool CachedZMQMessageWriter::WriteDelimitedTo(google::protobuf::MessageLite* message, google::protobuf::io::ZeroCopyOutputStream* raw_output){
    //Construct a coded output stream from the raw_output
    google::protobuf::io::CodedOutputStream out(raw_output);

    std::string type_name = message->GetTypeName();
    const int type_size = type_name.size();

    //Write the type size first
    out.WriteVarint32(type_size);
    //Write the type string
    out.WriteString(type_name);

    // Write the serialized size.
    const int size = message->ByteSize();
    out.WriteVarint32(size);

    uint8_t* buffer = out.GetDirectBufferForNBytesAndAdvance(size);
    if(buffer != NULL) {
        //Optimization: The message fits in one buffer, so use the faster direct-to-array serialization path.
        message->SerializeWithCachedSizesToArray(buffer);
    }else{
        //Slightly-slower path when the message is multiple buffers.
        message->SerializeWithCachedSizes(&out);
        if(out.HadError()){
            return false;
        }
    }
    return true;
}


bool CachedZMQMessageWriter::ReadDelimitedToStr(google::protobuf::io::ZeroCopyInputStream* raw_input, std::string* type, std::string* message){
    //Construct a coded input stream from the rawInput
    google::protobuf::io::CodedInputStream in(raw_input);

    //Read the type_size from the in.
    uint32_t type_size;
    if(!in.ReadVarint32(&type_size)){
        //No size, so return
        return false;
    }

    //Read the number of bytes as a string
    if(!in.ReadString(type, type_size)){
        std::cout << "Stream Error @ " << in.CurrentPosition() << " Reading " << type_size << std::endl;
        std::cout << "ERROR!" << std::endl;
        return false;
    }

    //Read the size from the in.
    uint32_t size;
    if(!in.ReadVarint32(&size)){
        //No size, so return
        return false;
    }

    //Read the number of bytes as a string
    if(!in.ReadString(message, size)){
        std::cout << "Stream Error @ " << in.CurrentPosition() << " Reading " << size << std::endl;
        std::cout << "ERROR!" << std::endl;
        return false;
    }
    return true;
}
