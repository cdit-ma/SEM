#include "server.h"

#include <iostream>

#include "sqlitedatabase.h"
#include "protohandler.h"
#include "../re_common/zmq/protoreceiver/protoreceiver.h"

Server::Server(const std::string& database_path, std::vector<std::string> addresses){
    //Construct database
	database_ = new SQLiteDatabase(database_path);

    //Construct receiver
	receiver_ = new zmq::ProtoReceiver();
    for(const auto& c : addresses){
        receiver_->Connect(c);
    }

    //Receieve all messages
    receiver_->Filter("");
}

void Server::AddProtoHandler(ProtoHandler* handler){
    if(!running_){
        handler_list_.push_back(handler);
    }else{
        std::cerr << "Could not add proto handler, receiver started." << std::endl;
    }
}

bool Server::Start(){
    bool success = false;
    if(!running_){
        for(const auto& handler : handler_list_){
            handler->ConstructTables(database_);
            handler->BindCallbacks(receiver_);
        }
        database_->Flush(true);
        std::cout << "* Constructed tables" << std::endl;
        running_ = true;
        success = receiver_->Start();
    }
    return success;
}

bool Server::Terminate(){
    if(running_){
        receiver_->Terminate();
        std::cout << "* Logged " << receiver_->GetRxCount() << " messages." << std::endl;
        delete receiver_;
        receiver_ = 0;

        for(auto handler : handler_list_){
            delete handler;
        }
        handler_list_.clear();
        delete database_;
        database_ = 0;
        running_ = false;
        return true;
    }
    return false;

}
