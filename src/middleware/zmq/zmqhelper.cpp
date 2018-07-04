#include "zmqhelper.h"
#include <iostream>
//Include ZMQ Headers
#include <zmq.hpp>

zmq::ZmqHelper& zmq::ZmqHelper::get_zmq_helper(){
    static zmq::ZmqHelper helper_;
    return helper_;
};

std::unique_ptr<zmq::socket_t> zmq::ZmqHelper::get_publisher_socket(){
    return std::unique_ptr<zmq::socket_t>(new zmq::socket_t(context_, ZMQ_PUB));
};

std::unique_ptr<zmq::socket_t> zmq::ZmqHelper::get_request_socket(){
    return std::unique_ptr<zmq::socket_t>(new zmq::socket_t(context_, ZMQ_REQ));
}

std::unique_ptr<zmq::socket_t> zmq::ZmqHelper::get_reply_socket(){
    return std::unique_ptr<zmq::socket_t>(new zmq::socket_t(context_, ZMQ_REP));
}

std::unique_ptr<zmq::socket_t> zmq::ZmqHelper::get_subscriber_socket(){
    auto socket = std::unique_ptr<zmq::socket_t>(new zmq::socket_t(context_, ZMQ_SUB));
    socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
    return std::move(socket);
};

int zmq::ZmqHelper::poll_socket(zmq::socket_t& socket, std::chrono::milliseconds timeout){
    return zmq::poll({{socket, 0, ZMQ_POLLIN, 0}}, timeout.count());
}