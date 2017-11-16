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
 
#include "protoreceiver.h"
#include <iostream>
#include "../monitor/monitor.h"

zmq::ProtoReceiver::ProtoReceiver(){
    //Setup ZMQ context
    context_ = new zmq::context_t(1);
}

void zmq::ProtoReceiver::Start(){
    if(callback_lookup_.empty()){
        std::cout << "Can't start zmq::ProtoReceiver: No ProtoHandler." << std::endl;
    }

    reciever_thread_ = new std::thread(&zmq::ProtoReceiver::RecieverThread, this);
    proto_convert_thread_ = new std::thread(&zmq::ProtoReceiver::ProtoConvertThread, this);
}

void zmq::ProtoReceiver::SetBatchMode(bool on, int size){
    batch_size_ = on ? size : 0;

    if(batch_size_ < 0){
        batch_size_ = 0;
    }
}

zmq::ProtoReceiver::~ProtoReceiver(){
    {
        //Gain the lock so we can notify and set our terminate flag.
        std::unique_lock<std::mutex> lock(queue_mutex_);
        terminate_proto_convert_thread_ = true;
        queue_lock_condition_.notify_all();
    }

    //Terminate the proto thread
    if(proto_convert_thread_){
        proto_convert_thread_->join();
        delete proto_convert_thread_;
    }

    delete context_;

    //Terminate the receiver thread
    if(reciever_thread_){
        reciever_thread_->join();
        delete reciever_thread_;
    }
}

void zmq::ProtoReceiver::AttachMonitor(zmq::Monitor* monitor, int event_type){
    std::unique_lock<std::mutex> lock(address_mutex_);
    if(monitor && socket_){
        //Attach monitor; using a new address
        monitor->MonitorSocket(socket_, GetNewMonitorAddress(), event_type);
    }
}

void zmq::ProtoReceiver::RecieverThread(){
    //Gain mutex lock to ensure we are the only thing working at this point.
    {
        std::unique_lock<std::mutex> lock(address_mutex_);
        //Setup our Subscriber socket
        socket_ = new zmq::socket_t(*context_, ZMQ_SUB);

        //Connect to all nodes on our network
        for (auto a : addresses_){
            Connect_(a);
        }
        for (auto f : filters_){
            Filter_(f);
        }
    }
    zmq::message_t *topic = new zmq::message_t();
    zmq::message_t *type = new zmq::message_t();
    zmq::message_t *data = new zmq::message_t();
    
    while(true){
		try{
            //Wait for next message
            socket_->recv(topic);
            socket_->recv(type);
			socket_->recv(data);
            
            //If we have a valid message
            if(type->size() > 0 && data->size() > 0){
                //Construct a string out of the zmq data
                std::string topic_str(static_cast<char *>(topic->data()), topic->size());
                std::string type_str(static_cast<char *>(type->data()), type->size());
                std::string msg_str(static_cast<char *>(data->data()), data->size());

                //Gain the lock so we can push this message onto our queue
                std::unique_lock<std::mutex> lock(queue_mutex_);
                rx_message_queue_.push(std::make_pair(type_str, msg_str));

                //Notify the condition when we have > 20 messages to process
                if(rx_message_queue_.size() > batch_size_){
                    queue_lock_condition_.notify_all();
                }
            }
        }catch(zmq::error_t &ex){
            if(ex.num() != ETERM){
                std::cerr << "ZMQ::Registrant::RegistrationThread: " << ex.what() << std::endl;
            }
			break;
        }
    }
    //Free our memory
    delete topic;
    delete type;
    delete data;
    delete socket_;
    socket_ = 0;
}


bool zmq::ProtoReceiver::RegisterNewProto(const google::protobuf::MessageLite &ml, std::function<void(google::protobuf::MessageLite*)> fn){
    std::string type = ml.GetTypeName();
    if(!callback_lookup_.count(type) && !proto_lookup_.count(type)){
        //Function pointer winraring
        callback_lookup_[type] = fn;
        auto construct_fn = [&ml](){return ml.New();};
        proto_lookup_[type] = construct_fn;
        return true;
    }
    return false;
}

google::protobuf::MessageLite* zmq::ProtoReceiver::ConstructMessage(std::string type, std::string data){
    if(proto_lookup_.count(type)){
        auto a = proto_lookup_[type]();
        a->ParseFromString(data);
        return a;
    }
    return 0;
}


void zmq::ProtoReceiver::ProtoConvertThread(){
    //Update loop.
    while(true){
        std::queue<std::pair<std::string, std::string> > replace_queue;
        {
            //Obtain lock for the queue
            std::unique_lock<std::mutex> lock(queue_mutex_);
            if(terminate_proto_convert_thread_){
                return;
            }
            //Wait for the condition to be notified
            queue_lock_condition_.wait(lock);
            
            //Now that we have access, swap out queues and release the mutex
            if(!rx_message_queue_.empty()){
                rx_message_queue_.swap(replace_queue);
            }
        }

        while(!replace_queue.empty()){
            std::string type = replace_queue.front().first;
            std::string msg = replace_queue.front().second;

            if(callback_lookup_.count(type)){
                auto message = ConstructMessage(type, msg);
                if(message){
                    callback_lookup_[type](message);
                }
            }
            replace_queue.pop();
        }
    }
}

void zmq::ProtoReceiver::Connect_(std::string address){
    //If we have a reciever_thread_ active we can directly interact
    try{
        if(socket_){
            socket_->connect(address.c_str());
        }
    }
    catch(zmq::error_t ex){
        std::cout << zmq_strerror(ex.num()) << std::endl;
    }    
}

void zmq::ProtoReceiver::Filter_(std::string topic_filter){
    //If we have a reciever_thread_ active we can directly interact
    try{
        if(socket_){
            //Subscribe to specific topic
            socket_->setsockopt(ZMQ_SUBSCRIBE, topic_filter.c_str(), topic_filter.size());
        }
    }
    catch(zmq::error_t ex){
        std::cout << zmq_strerror(ex.num()) << std::endl;
    }    

}
void zmq::ProtoReceiver::Filter(std::string filter){
    std::unique_lock<std::mutex> lock(address_mutex_);

    if(!reciever_thread_){
        //Append to addresses.
        filters_.push_back(filter);        
    }else{
        Filter_(filter);
    }
}

void zmq::ProtoReceiver::Connect(std::string address){
    std::unique_lock<std::mutex> lock(address_mutex_);

    if(!reciever_thread_){
        //Append to addresses.
        addresses_.push_back(address);        
    }else{
        Connect_(address);
    }
}