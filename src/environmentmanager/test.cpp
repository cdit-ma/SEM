#include <zmq.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <climits>
#include <tuple>

#include "environmentmessage/environmentmessage.pb.h"

long clock_ = 0;

long Tick(){
    clock_++;
    return clock_;
}

long SetClock(long incoming_time){
    clock_ = std::max(incoming_time, clock_) + 1;
    return clock_;
}

long GetClock(){
    return clock_;
}

void Send(zmq::socket_t* socket, std::string part_one, std::string part_two){
    std::string lamport_string = std::to_string(Tick());
    zmq::message_t lamport_time_msg(lamport_string.begin(), lamport_string.end());
    zmq::message_t part_one_msg(part_one.begin(), part_one.end());
    zmq::message_t part_two_msg(part_two.begin(), part_two.end());

    try{
        socket->send(part_one_msg, ZMQ_SNDMORE);
        socket->send(lamport_time_msg, ZMQ_SNDMORE);
        socket->send(part_two_msg);
    }
    catch(std::exception e){
        std::cout << e.what() << std::endl;
    }
}

std::tuple<std::string, long, std::string> Receive(zmq::socket_t* socket){
    zmq::message_t request_type_msg;
    zmq::message_t lamport_time_msg;
    zmq::message_t request_contents_msg;
    try{
        socket->recv(&request_type_msg);
        socket->recv(&lamport_time_msg);
        socket->recv(&request_contents_msg);
    }
    catch(zmq::error_t error){
        //TODO: Throw this further up
        std::cout << error.what() << std::endl;
    }
    std::string type(static_cast<const char*>(request_type_msg.data()), request_type_msg.size());
    std::string contents(static_cast<const char*>(request_contents_msg.data()), request_contents_msg.size());

    //Update and get current lamport time
    std::string incoming_time(static_cast<const char*>(lamport_time_msg.data()), lamport_time_msg.size());
    long lamport_time = SetClock(std::stol(incoming_time));

    return std::make_tuple(type, lamport_time, contents);
}

int main(int argc, char **argv){

    clock_ = 0;

    std::cout << "Started" << std::endl;

    zmq::context_t context(1);
    zmq::socket_t sub(context, ZMQ_SUB);

    sub.connect("tcp://192.168.111.230:22334");
    sub.setsockopt(ZMQ_SUBSCRIBE, "", 0);

    std::cout << "Subscribed" << std::endl;

    zmq::message_t message;
    sub.recv(&message);
    auto endpoint = std::string(static_cast<const char*>(message.data()), message.size());
    sub.close();

    std::cout << "sub1 < " << endpoint << std::endl;

    zmq::socket_t* req = new zmq::socket_t(context, ZMQ_REQ);
    req->connect(endpoint);

    std::cout << "Connected to endpoint: " << endpoint << std::endl;

    bool flag = argc == 2;

    if(flag){

        std::string deployment_name(argv[1]);

        Send(req, "DEPLOYMENT", "asdf");

        std::cout << "1> " << "DEPLOYMENT" << ": " << "asdf" << std::endl;

        auto rec1 = Receive(req);

        auto type_str = std::get<0>(rec1);
        auto hb_endpoint = std::get<2>(rec1);
        std::cout << "2< " << type_str << ": " << hb_endpoint << std::endl;

        zmq::socket_t* hb_soc = new zmq::socket_t(context, ZMQ_REQ);
        hb_soc->connect(hb_endpoint);

        std::cout << "Connected to heartbeat endpoint" << std::endl;


        for(int i =0; i<2; i++){

            std::string m1("ASSIGNMENT_REQUEST");

            std::string m2;
            m2 += deployment_name;
            m2 += ":Component";
            m2 += std::to_string(i);

            Send(hb_soc, m1, m2);
            std::cout << "3> " << m1 << ": " << m2 << std::endl;

            auto rep2 = Receive(hb_soc);
            auto reply = std::get<0>(rep2);
            auto reply2 = std::get<2>(rep2);
            std::cout << "4< " << reply << ": " << reply2 << std::endl;
        }

        std::string endmessage("END_ASSIGNMENT");
        std::string endmessage2("");
        Send(hb_soc, endmessage, endmessage2);

        std::cout << "5> " << endmessage << ": " << endmessage2 << std::endl;


        auto rep3 = Receive(hb_soc);
        auto reply = std::get<0>(rep3);
        auto reply2 = std::get<0>(rep3);
        
        std::cout << "6< " << reply << ": " << reply2 << std::endl;

        while(true){

            Send(hb_soc, "HEARTBEAT", "HEARTBEAT");
            auto hb_rep = Receive(hb_soc);

            auto hb_rep_string = std::get<0>(hb_rep);

            std::cout << "< " << hb_rep_string << std::endl;
            std::cout << "< " << std::get<1>(hb_rep) << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }

    // else{
    //     std::string type("QUERY");
    //     zmq::message_t type_msg(type.begin(), type.end());

    //     std::cout << "created message type" << std::endl;

    //     std::string query("PORT:");
    //     query += argv[2];
    //     zmq::message_t query_msg(query.begin(), query.end());

    //     std::cout << "created query_msg message" << std::endl;


    //     req.send(type_msg, ZMQ_SNDMORE);
    //     std::cout << "sent type message" << std::endl;
    //     req.send(query_msg);
    //     std::cout << "sent query_msg" << std::endl;

    //     zmq::message_t inbound_type;
    //     req.recv(&inbound_type);
    //     zmq::message_t inbound;
    //     req.recv(&inbound);

    //     std::cout << "recieved inbound message" << std::endl;

    //     auto type_str = std::string(static_cast<const char*>(inbound_type.data()), inbound_type.size());

    //     std::cout << "Inbound message type: " << type_str << std::endl;

    //     auto query_response = std::string(static_cast<const char*>(inbound.data()), inbound.size());

    //     std::cout << query_response << std::endl;

    // }
}
