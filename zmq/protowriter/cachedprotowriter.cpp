/* re_common
 * Copyright (C) 2016-2017 The University of Adelaide
 *
 * This file is part of "re_common"
 *
 * "re_common" is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * "re_common" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
 
#include "cachedprotowriter.h"

#include <cstdio>
#include <iostream>
#include <fstream>

#ifdef _WIN32
#include <io.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif
#include <fcntl.h>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <boost/filesystem.hpp>

int getWriteFileDesc(const char* filename) {
    int filedesc;
#ifdef _WIN32
    _sopen_s(&filedesc, filename, _O_APPEND | _O_CREAT | _O_WRONLY, _SH_DENYNO, _S_IWRITE | _S_IREAD);
#else
    filedesc = open(filename, O_APPEND | O_CREAT | O_WRONLY, S_IWRITE | S_IREAD);
#endif
    return filedesc; 
}

int getReadFileDesc(const char* filename) {
    int filedesc;
#ifdef _WIN32
    _sopen_s(&filedesc, filename, _O_RDONLY, _SH_DENYNO, _S_IWRITE | _S_IREAD);
#else
    filedesc = open(filename, O_RDONLY, S_IWRITE | S_IREAD);
#endif
    return filedesc; 
}

zmq::CachedProtoWriter::CachedProtoWriter(int cache_count) : zmq::ProtoWriter(){
    cache_count_ = cache_count;
    //Get a temporary file location for our cached files
    auto temp = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    temp_file_path_ = temp.string();

    //Start the writer thread
    writer_future_ = std::async(&zmq::CachedProtoWriter::WriteQueue, this);
}   

zmq::CachedProtoWriter::~CachedProtoWriter(){
    std::cerr << "CachedProtoWriter()()" << std::endl;
    Terminate();
}

//Takes ownership of message
bool zmq::CachedProtoWriter::PushMessage(const std::string& topic, std::unique_ptr<google::protobuf::MessageLite> message){
    std::unique_lock<std::mutex> lock(mutex_);
    if(writer_future_.valid()){
        if(message){
            //Gain the lock
            std::unique_lock<std::mutex> lock(queue_mutex_);
            //Push the message onto the queue
            write_queue_.emplace(std::make_pair(topic, std::move(message)));
            log_count_ ++;
            if(write_queue_.size() >= cache_count_){
                //Notify the writer_thread to flush the queue if we have hit our write limit
                queue_lock_condition_.notify_all();
            }
            return true;
        }else{
            std::cerr << "zmq::CachedProtoWriter::PushMessage() Given a null message" << std::endl;
        }
    }
    return false;
}

void zmq::CachedProtoWriter::Terminate(){
    std::cerr << "CachedProtoWriter()::Terminate()" << std::endl;
    std::unique_lock<std::mutex> lock(mutex_);
    if(writer_future_.valid()){
        {
            //Gain the lock so we can notify and set our terminate flag.
            std::unique_lock<std::mutex> lock(queue_mutex_);
            writer_terminate_ = true;
            queue_lock_condition_.notify_all();
        }

        //Wait for the writer_thread to finish
        writer_future_.get();

        //Read the messages from the queue
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            if(written_to_disk_count){
                auto messages = ReadMessagesFromFile(temp_file_path_);
                if(messages.size() == written_to_disk_count){
                    std::cout << "* zmq::CachedProtoWriter: Read all " << messages.size() << " messages in cache file '" << temp_file_path_ << "'" << std::endl;
                }else{
                    std::cerr << "* zmq::CachedProtoWriter: Could only read " << messages.size() << "/" << written_to_disk_count  << "messages in cache file '" << temp_file_path_ << "'" << std::endl;
                }

                //Send the messages serialized in the temp file
                while(!messages.empty()){
                    auto& s = messages.front();
                    if(s){
                        zmq::ProtoWriter::PushString(s->topic, s->type, s->data);
                    }
                    messages.pop();
                }
            }
        }
    
        //Send the Messages still in the write queue
        while(!write_queue_.empty()){
            const auto& topic = write_queue_.front().first;
            auto& message = write_queue_.front().second;
            zmq::ProtoWriter::PushMessage(topic, std::move(message));
            write_queue_.pop();
        }

        //Remove the temp file
        std::remove(temp_file_path_.c_str());
        running = false;
    }else{
        std::cerr << "NOT VALID FUTURE FRONDO" << std::endl;
    }

    //Terminate the base class
    zmq::ProtoWriter::Terminate();
}

void zmq::CachedProtoWriter::WriteQueue(){
    while(true){
        std::queue<std::pair<std::string, std::unique_ptr<google::protobuf::MessageLite> > > replace_queue;
        {
            //Obtain lock for the queue
            std::unique_lock<std::mutex> lock(queue_mutex_);
            //Wait for notify
            queue_lock_condition_.wait(lock, [this]{return writer_terminate_ || write_queue_.size();});
            
            if(writer_terminate_){
                return;
            }
            //Swap the queue
            write_queue_.swap(replace_queue);
        }
        
        if(replace_queue.size()){
            int filedesc = getWriteFileDesc(temp_file_path_.c_str());
            if(filedesc < 0){
                std::cerr << "Failed to open temp file '" << temp_file_path_ << "' to write." << std::endl;
                break;
            }

            //Make an output stream using the file
            ::google::protobuf::io::FileOutputStream raw_output(filedesc);
            raw_output.SetCloseOnDelete(true);
            
            int write_count = 0;
            
            //Write all messages in queue
            while(!replace_queue.empty()){
                const auto& topic = replace_queue.front().first;
                auto message = std::move(replace_queue.front().second);
                
                if(message){
                    if(WriteDelimitedTo(topic, *message, &raw_output)){
                        write_count ++;
                    }else{
                        std::cerr << "zmq::CachedProtoWriter::WriteQueue(): Error writing message to temp file '" << temp_file_path_ << "'" << std::endl;
                    }
                }else{
                    std::cerr << "zmq::CachedProtoWriter::WriteQueue(): got NULL message." << std::endl;
                }
                replace_queue.pop();
            }

            //Obtain lock for the queue
            std::unique_lock<std::mutex> lock(queue_mutex_);
            written_to_disk_count += write_count;
        }
    }
}

std::queue< std::unique_ptr<Message_Struct> > zmq::CachedProtoWriter::ReadMessagesFromFile(const std::string& file_path){
    std::queue< std::unique_ptr<Message_Struct> > queue;

    int filedesc = getReadFileDesc(file_path.c_str());

    if(filedesc < 0){
        std::cerr << "Failed to open file '" << file_path << "' to read." << std::endl;
        return queue;
    }


    //Make an input stream using the file
    ::google::protobuf::io::FileInputStream raw_input(filedesc);
    raw_input.SetCloseOnDelete(true);
    
    while(true){
        //Allocate a new string to store the read data
        auto message_struct = std::unique_ptr<Message_Struct>(new Message_Struct());

        //Read the proto encoded string into our message and queue if successful
        if(ReadDelimitedToStr(&raw_input, message_struct->topic, message_struct->type, message_struct->data)){
            queue.emplace(std::move(message_struct));
        }else{
            break;
        }
    }
    return std::move(queue);
}

bool zmq::CachedProtoWriter::WriteDelimitedTo(const std::string& topic, const google::protobuf::MessageLite& message, google::protobuf::io::ZeroCopyOutputStream* raw_output){
    //Construct a coded output stream from the raw_output
    google::protobuf::io::CodedOutputStream out(raw_output);

    auto&& type_name = message.GetTypeName();
    const uint32_t type_size = static_cast<uint32_t>(type_name.size());
    const uint32_t topic_size = static_cast<uint32_t>(topic.size());

    out.WriteVarint32(topic_size);
    out.WriteString(topic);

    //Write the type size first
    out.WriteVarint32(type_size);
    //Write the type string
    out.WriteString(type_name);

    // Write the serialized size.
    const uint32_t size = message.ByteSize();
    out.WriteVarint32(size);

    uint8_t* buffer = out.GetDirectBufferForNBytesAndAdvance(size);
    if(buffer != NULL) {
        //Optimization: The message fits in one buffer, so use the faster direct-to-array serialization path.
        message.SerializeWithCachedSizesToArray(buffer);
    }else{
        //Slightly-slower path when the message is multiple buffers.
        message.SerializeWithCachedSizes(&out);
        if(out.HadError()){
            return false;
        }
    }
    return true;
}


bool zmq::CachedProtoWriter::ReadDelimitedToStr(google::protobuf::io::ZeroCopyInputStream* raw_input, std::string& topic, std::string& type, std::string& message){
    //Construct a coded input stream from the rawInput
    google::protobuf::io::CodedInputStream in(raw_input);

    //Read topic size
    uint32_t topic_size;
    if(!in.ReadVarint32(&topic_size)){
        //No size, so return
        return false;
    }

    //Read the number of bytes as a string
    if(!in.ReadString(&topic, topic_size)){
        std::cout << "Stream Error @ " << in.CurrentPosition() << " Reading " << topic_size << std::endl;
        std::cout << "ERROR!" << std::endl;
        return false;
    }

    //Read the type_size from the in.
    uint32_t type_size;
    if(!in.ReadVarint32(&type_size)){
        //No size, so return
        return false;
    }

    //Read the number of bytes as a string
    if(!in.ReadString(&type, type_size)){
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
    if(!in.ReadString(&message, size)){
        std::cout << "Stream Error @ " << in.CurrentPosition() << " Reading " << size << std::endl;
        std::cout << "ERROR!" << std::endl;
        return false;
    }
    return true;
}
