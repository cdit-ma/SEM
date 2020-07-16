#include "server.h"

#include <iostream>

#include "sqlitedatabase.h"
#include "protohandler.h"
#include "protoreceiver.h"

#ifndef DISABLE_HARDWARE_HANDLER
#include "protohandlers/systemevent/protohandler.h"
#endif

#ifndef DISABLE_MODEL_HANDLER
#include "protohandlers/modelevent/protohandler.h"
#endif

Server::Server(const std::string& database_path, const std::vector<std::string>& addresses){
    proto_receiver_ = std::unique_ptr<zmq::ProtoReceiver>(new zmq::ProtoReceiver());
    database_ = std::unique_ptr<SQLiteDatabase>(new SQLiteDatabase(database_path));

    

    //Add our proto handlers and start the server
    #ifndef DISABLE_HARDWARE_HANDLER
    AddProtoHandler(std::unique_ptr<ProtoHandler>(new SystemEvent::ProtoHandler(*database_)));
    #endif

    #ifndef DISABLE_MODEL_HANDLER
    AddProtoHandler(std::unique_ptr<ProtoHandler>(new ModelEvent::ProtoHandler(*database_)));
    #endif

    //Recieve all messages
    proto_receiver_->Filter("");

    //Connect to all addresses
    for(const auto& address : addresses){
        proto_receiver_->Connect(address);
    }

    auto statement_size = database_->Flush();
    std::cout << "* Constructed " << statement_size << " tables" << std::endl;
}


SQLiteDatabase& Server::GetDatabase(){
    return *database_;
}

void Server::AddProtoHandler(std::unique_ptr<ProtoHandler> proto_handler){
    std::lock_guard<std::mutex> lock(mutex_);
    proto_handler->BindCallbacks(*proto_receiver_);
    proto_handlers_.emplace_back(std::move(proto_handler));
}

Server::~Server(){
    //Shutdown the receiver
    proto_receiver_.reset();

    //Destroy the proto handlers
    proto_handlers_.clear();

    //Destroy the databases
    database_.reset();
}