#include "zmqreceiver.h"
#include <iostream>


ZMQReceiver::ZMQReceiver(std::vector<std::string> addrs, std::string port, int batch_size){
    port_ = port;
    batch_size_ = batch_size;

    for(auto addr : addrs){
        std::string temp = "tcp://" + addr + ":" + port;
        addresses_.push_back(temp);
    }

    //Setup ZMQ context
    context_ = new zmq::context_t(1);
    
    //Construct our terminate socket
    term_socket_ = new zmq::socket_t(*context_, ZMQ_PUB);
	term_socket_->bind("inproc://term_signal");
}

void ZMQReceiver::Start(){
    if(!process_callback_){
        std::cout << "Can't start ZMQReceiver: No ProtoHandler." << std::endl;
    }

    reciever_thread_ = new std::thread(&ZMQReceiver::RecieverThread, this);
    proto_convert_thread_ = new std::thread(&ZMQReceiver::ProtoConvertThread, this);
}


void ZMQReceiver::TerminateReceiver(){
    //Set our terminate
    terminate_reciever_ = true;

    //Send a blank message
    term_socket_->send(zmq::message_t());

    //Wait for the reciever_thread_ to finish
    reciever_thread_->join();

    delete term_socket_;
    delete context_;
}

ZMQReceiver::~ZMQReceiver(){
    {
        //Gain the lock so we can notify and set our terminate flag.
        std::unique_lock<std::mutex> lock(queue_mutex_);
        terminate_proto_convert_thread_ = true;
        queue_lock_condition_.notify_all();
    }

    //The reciever_thread_ should have finished (terminate_reciever is called prior to sql controller)
    delete reciever_thread_;

    //Wait for the sql_thread to terminate
    proto_convert_thread_->join();
    delete proto_convert_thread_;
    
    while(!types_.empty()){
        delete types_.front();
        types_.pop();
    }
}

void ZMQReceiver::SetProtoHandlerCallback(std::function<void(google::protobuf::MessageLite*)> fn){
    process_callback_ = fn;
}

void ZMQReceiver::RecieverThread(){
    //Setup our Subscriber socket
    zmq::socket_t socket(*context_, ZMQ_SUB);
	
    //Subscribe to everything
    socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);

    //Connect to our terminate inprocess socket
    socket.connect("inproc://term_signal");

    //Connect to all nodes on our network
	for (auto a : addresses_){
		socket.connect(a.c_str());
	}
	
    zmq::message_t *type = new zmq::message_t();
    zmq::message_t *data = new zmq::message_t();
    
    while(!terminate_reciever_){
		try{
            //Wait for next message
            socket.recv(type);
            if(terminate_reciever_){
                break;
            }
			socket.recv(data);
            if(terminate_reciever_){
                break;
            }


            //If we have a valid message
            if(type->size() > 0 && data->size() > 0){
                //Construct a string out of the zmq data
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
        }catch(zmq::error_t ex){
            //Do nothing with an error.
			continue;
        }
    }
    //Free our memory
    delete data;
}


void ZMQReceiver::RegisterNewProto(google::protobuf::MessageLite* ml){
    std::string type = ml->GetTypeName();
    //Function pointer winraring
    auto fn = [ml](){return ml->New();};
    proto_lookup_[type] = fn;
    types_.push(ml);
    //Remember to destroy elements in types_ in destructor
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

            if(process_callback_){
                auto message = ConstructMessage(type, msg);
                if(message){
                    process_callback_(message);
                }
            }
            replace_queue.pop();
        }
    }
}