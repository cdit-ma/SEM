#include "zmqrxmessage.h"

#include <iostream>

zmq_rxMessage::zmq_rxMessage(rxMessageInt* component, zmq::context_t* context, std::string endpoint){
    this->component_ = component;
    this->context_ = context;
    //Construct a socket!
    this->socket_ = new zmq::socket_t(*context, ZMQ_SUB);
    this->socket_->connect(endpoint);

    rec_thread_ = new std::thread(&zmq_rxMessage::recieve, this);
}

void rxMessage(Message* message){
    component_->rxMessage(message);
}

void zmq_rxMessage::recieve(){
    zmq::message_t *data = new zmq::message_t();
    
    while(true){
		try{
            //Wait for next message
			socket.recv(data);

            //If we have a valid message
            if(data->size() > 0){
                //Construct a string out of the zmq data
                std::string msg_str(static_cast<char *>(data->data()), data->size());
                
                //Construct a Protobuf object
                proto::Message* message = new proto::Message();

                if (message->ParseFromString(msg_str)){
                    Message* modelMessage = proto_to_message(message);
                    this->rxMessage(modelMessage);
                }
            }
        }catch(zmq::error_t ex){
            //Do nothing with an error.
			continue;
        }
    }

}