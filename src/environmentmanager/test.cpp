
#include <zmq.hpp>
#include <iostream>
#include <thread>
#include <chrono>

int main(){

    zmq::context_t context(1);
    zmq::socket_t sub(context, ZMQ_SUB);

    sub.connect("tcp://localhost:22334");
    sub.setsockopt(ZMQ_SUBSCRIBE, "", 0);

    zmq::message_t message;
    sub.recv(&message);
    sub.close();

    zmq::socket_t req(context, ZMQ_REQ);
    auto endpoint = std::string(static_cast<const char*>(message.data()), message.size());
    req.connect(endpoint);

    std::string deployment("");
    zmq::message_t deployment_msg(deployment.begin(), deployment.end());

    req.send(deployment_msg);

    zmq::message_t inbound;
    req.recv(&inbound);

    auto hb_endpoint = std::string(static_cast<const char*>(inbound.data()), inbound.size());

    std::cout << hb_endpoint << std::endl;


    zmq::socket_t hb_soc(context, ZMQ_REQ);
    hb_soc.connect(hb_endpoint);

    while(true){

        std::string hb_str("HB");
        zmq::message_t hb_message(hb_str.begin(), hb_str.end());
        hb_soc.send(hb_message);

        zmq::message_t ack;
        hb_soc.recv(&ack);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

}