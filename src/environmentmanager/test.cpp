
#include <zmq.hpp>
#include <iostream>
#include <thread>
#include <chrono>

#include "../nodemanager/controlmessage/controlmessage.pb.h"

int main(int argc, char **argv){

    std::cout << "Started" << std::endl;

    zmq::context_t context(1);
    zmq::socket_t sub(context, ZMQ_SUB);

    sub.connect("tcp://192.168.111.230:22334");
    sub.setsockopt(ZMQ_SUBSCRIBE, "", 0);

    std::cout << "Subscribed" << std::endl;

    zmq::message_t message;
    sub.recv(&message);
    sub.close();

    std::cout << "Recieved and closed sub" << std::endl;    

    zmq::socket_t req(context, ZMQ_REQ);
    auto endpoint = std::string(static_cast<const char*>(message.data()), message.size());
    req.connect(endpoint);

    std::cout << "Connected to endpoint: " << endpoint << std::endl;

    bool flag = argc == 2;

    if(flag){
        std::string type("DEPLOYMENT");
        zmq::message_t type_msg(type.begin(), type.end());

        std::cout << "created message" << std::endl;

        std::string deployment_name(argv[1]);
        NodeManager::ControlMessage* deployment = new NodeManager::ControlMessage();
        deployment->set_host_name(deployment_name);

        std::string out = deployment->SerializeAsString();

        zmq::message_t deployment_msg(out.begin(), out.end());

        std::cout << "created deployment message" << std::endl;


        req.send(type_msg, ZMQ_SNDMORE);
        std::cout << "sent type message" << std::endl;
        req.send(deployment_msg);
        std::cout << "sent deployment message" << std::endl;


        zmq::message_t inbound_type;
        req.recv(&inbound_type);
        zmq::message_t inbound;
        req.recv(&inbound);

        std::cout << "recieved inbound message" << std::endl;

        auto type_str = std::string(static_cast<const char*>(inbound_type.data()), inbound_type.size());

        std::cout << "Inbound message type: " << type_str << std::endl;

        auto hb_endpoint = std::string(static_cast<const char*>(inbound.data()), inbound.size());

        std::cout << "Got heartbeat endpoint: " << hb_endpoint << std::endl;

        zmq::socket_t hb_soc(context, ZMQ_REQ);
        hb_soc.connect(hb_endpoint);

        std::cout << "connected to heartbeat endpoint" << std::endl;


        for(int i =0; i<10; i++){

            std::string m1("ASSIGNMENT_REQUEST");
            zmq::message_t m1m(m1.begin(), m1.end());

            std::string m2;
            m2 += deployment_name;
            m2 += ":Component";
            m2 += std::to_string(i);
            zmq::message_t m2m(m2.begin(), m2.end());

            hb_soc.send(m1m, ZMQ_SNDMORE);
            hb_soc.send(m2m);

            zmq::message_t asdf;
            zmq::message_t asdf2;
            hb_soc.recv(&asdf);
            hb_soc.recv(&asdf2);
            auto reply = std::string(static_cast<const char*>(asdf.data()), asdf.size());
            auto reply2 = std::string(static_cast<const char*>(asdf2.data()), asdf2.size());
            std::cout << reply << std::endl;
            std::cout << reply2 << std::endl;
        }

        std::string endmessage("END_ASSIGNMENT");
        zmq::message_t endm(endmessage.begin(), endmessage.end());

        hb_soc.send(endm, ZMQ_SNDMORE);
        hb_soc.send(endm);

        std::cout << "sent assignment end" << std::endl;

        zmq::message_t endm2;
        hb_soc.recv(&endm2);
        hb_soc.recv(&endm2);
        
        std::cout << "got end ack" << std::endl;

        while(true){

            std::string hb_str("HB");
            zmq::message_t hb_message(hb_str.begin(), hb_str.end());
            hb_soc.send(hb_message);

            zmq::message_t ack;
            hb_soc.recv(&ack);

            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "beat" << std::endl;
        }

    }

    else{
        std::string type("QUERY");
        zmq::message_t type_msg(type.begin(), type.end());

        std::cout << "created message type" << std::endl;

        std::string query("PORT:");
        query += argv[2];
        zmq::message_t query_msg(query.begin(), query.end());

        std::cout << "created query_msg message" << std::endl;


        req.send(type_msg, ZMQ_SNDMORE);
        std::cout << "sent type message" << std::endl;
        req.send(query_msg);
        std::cout << "sent query_msg" << std::endl;

        zmq::message_t inbound_type;
        req.recv(&inbound_type);
        zmq::message_t inbound;
        req.recv(&inbound);

        std::cout << "recieved inbound message" << std::endl;

        auto type_str = std::string(static_cast<const char*>(inbound_type.data()), inbound_type.size());

        std::cout << "Inbound message type: " << type_str << std::endl;

        auto query_response = std::string(static_cast<const char*>(inbound.data()), inbound.size());

        std::cout << query_response << std::endl;

    }
}
