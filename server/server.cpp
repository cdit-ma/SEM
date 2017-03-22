#include "server.h"

#include <iostream>

#include "sqlitedatabase.h"
#include "protohandler.h"
#include "../re_common/zmq/protoreceiver/protoreceiver.h"

Server::Server(std::string database_path, std::vector<std::string> addresses){
	database_ = new SQLiteDatabase(database_path);

	receiver_ = new zmq::ProtoReceiver();
    for(auto c : addresses){
        receiver_->Connect(c);
    }

    //Set no filter
    receiver_->Filter("");
}

Server::~Server(){
    delete receiver_;
    delete database_;
    for(auto handler : handler_list_){
        delete handler;
    }
}

void Server::AddProtoHandler(ProtoHandler* handler){
    if(!started_){
        handler->ConstructTables(database_);
        handler->BindCallbacks(receiver_);
        handler_list_.push_back(handler);
    }else{
        std::cerr << "Could not add proto handler, receiver started." << std::endl;
    }
}

void Server::Start(){
    started_ = true;
    receiver_->Start();
}
