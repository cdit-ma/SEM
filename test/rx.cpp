#include <memory>
#include <iostream>
#include <chrono>
#include <thread>
#include <list>
#include <algorithm>

#include <re_common/zmq/protoreplier/protoreplier.hpp>

#include <proto/controlmessage/controlmessage.pb.h>
#include "../zmq/zmqutils.hpp"
int recv = 0;

std::unique_ptr<NodeManager::ControlMessage> Func(const NodeManager::Node& message){
    recv++;
    //std::cerr << "CALLBACK FAM" << std::endl;
    auto r_mess = std::unique_ptr<NodeManager::ControlMessage>(new NodeManager::ControlMessage());
    r_mess->set_host_name("HOSTNAME");

    //std::cerr << "GOT MESSAGE: "<< std::endl;
    //std::this_thread::sleep_for(std::chrono::milliseconds(200));
    //std::cerr << "At 2 sec: "<< std::endl;
    //std::this_thread::sleep_for(std::chrono::milliseconds(200));



    if(recv == 95){
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }else if(recv % 50 == 0){
        throw std::runtime_error("LOL GOT 50");
    }else{
    }
    //std::this_thread::sleep_for(std::chrono::milliseconds(1));

    return std::move(r_mess);
}

std::unique_ptr<NodeManager::ControlMessage> Func2(const NodeManager::ControlMessage& message){
    std::cerr << "CALLBACK FAM2" << std::endl;
    auto r_mess = std::unique_ptr<NodeManager::ControlMessage>(new NodeManager::ControlMessage(message));
    return std::move(r_mess);
}

int main(int argc, char** argv){
    std::string function_name("lelbs");
    std::string function2_name("lelbs2");
    
    std::string address("tcp://192.168.111.187:13373");

    auto proto_replier = std::unique_ptr<zmq::ProtoReplier>(new zmq::ProtoReplier());


    proto_replier->RegisterProtoCallback<NodeManager::Node, NodeManager::ControlMessage>(function_name, Func);
    proto_replier->RegisterProtoCallback<NodeManager::ControlMessage, NodeManager::ControlMessage>(function2_name, Func2);


    proto_replier->Bind(address);
    proto_replier->Start();

    std::this_thread::sleep_for(std::chrono::seconds(1000));
    std::cerr << "SHUTTING DOWN" << std::endl;
    proto_replier->Terminate();
    std::cerr << "SHUT DOWN" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cerr << "STARTING UP" << std::endl;
    proto_replier->Start();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return 0;
}