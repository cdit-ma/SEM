#include "zmqreceiver.h"
#include <iostream>
#include <zmq.hpp>

ZMQReceiver::ZMQReceiver(int batch_size){
    batch_size_ = batch_size;
    
    //Setup ZMQ context
    context_ = new zmq::context_t(1);
}

void ZMQReceiver::Start(){
    if(callback_lookup_.empty()){
        std::cout << "Can't start ZMQReceiver: No ProtoHandler." << std::endl;
    }

    reciever_thread_ = new std::thread(&ZMQReceiver::RecieverThread, this);
    proto_convert_thread_ = new std::thread(&ZMQReceiver::ProtoConvertThread, this);
}

ZMQReceiver::~ZMQReceiver(){
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

void ZMQReceiver::RecieverThread(){
    //Gain mutex lock to ensure we are the only thing working at this point.
    {
        std::unique_lock<std::mutex> lock(address_mutex_);
        //Setup our Subscriber socket
        socket_ = new zmq::socket_t(*context_, ZMQ_SUB);
        
        //Filter against *
        Filter_("*");
        
        //Connect to all nodes on our network
        for (auto a_p : addresses_){
            auto a = a_p.first;
            auto f = a_p.second;
            Connect_(a);
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


void ZMQReceiver::RegisterNewProto(const google::protobuf::MessageLite &ml, std::function<void(google::protobuf::MessageLite*)> fn){
    std::string type = ml.GetTypeName();
    //Function pointer winraring
    callback_lookup_[type] = fn;

    auto construct_fn = [&ml](){return ml.New();};
    proto_lookup_[type] = construct_fn;
}

google::protobuf::MessageLite* ZMQReceiver::ConstructMessage(std::string type, std::string data){
    if(proto_lookup_.count(type)){
        auto a = proto_lookup_[type]();
        a->ParseFromString(data);
        return a;
    }
    return 0;
}


void ZMQReceiver::ProtoConvertThread(){
    //Update loop.
    while(!terminate_proto_convert_thread_){
        std::queue<std::pair<std::string, std::string> > replace_queue;
        {
            //Obtain lock for the queue
            std::unique_lock<std::mutex> lock(queue_mutex_);
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

void ZMQReceiver::Connect_(std::string address){
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

void ZMQReceiver::Filter_(std::string topic_filter){
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

void ZMQReceiver::Connect(std::string address, std::string topic_filter){
    auto p = std::pair<std::string, std::string>(address, topic_filter);
    //Obtain lock for the queue
    std::unique_lock<std::mutex> lock(address_mutex_);

    if(!reciever_thread_){
        //Append to addresses.
        addresses_.push_back(p);        
    }else{
        Connect_(address);
        Filter_(topic_filter);
    }
}