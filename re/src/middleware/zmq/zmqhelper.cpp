#include "zmqhelper.h"
#include <iostream>
//Include ZMQ Headers
#include <zmq.hpp>

zmq::Helper& zmq::get_zmq_helper(){
    static zmq::Helper helper_;
    return helper_;
};

std::unique_ptr<zmq::context_t> zmq::Helper::get_context(){
    return std::unique_ptr<zmq::context_t>(new zmq::context_t(1));
}

std::unique_ptr<zmq::socket_t> zmq::Helper::get_publisher_socket(){
    return std::unique_ptr<zmq::socket_t>(new zmq::socket_t(context_, ZMQ_PUB));
};

std::unique_ptr<zmq::socket_t> zmq::Helper::get_request_socket(){
    auto socket = std::unique_ptr<zmq::socket_t>(new zmq::socket_t(context_, ZMQ_SUB));
    socket->setsockopt(ZMQ_LINGER, 0);
    return std::move(socket);
}

std::unique_ptr<zmq::socket_t> zmq::Helper::get_socket(zmq::context_t& context, const int socket_type){
    auto socket = std::unique_ptr<zmq::socket_t>(new zmq::socket_t(context, socket_type));
    return std::move(socket);
}

std::unique_ptr<zmq::socket_t> zmq::Helper::get_reply_socket(){
    return std::unique_ptr<zmq::socket_t>(new zmq::socket_t(context_, ZMQ_REP));
}

std::unique_ptr<zmq::socket_t> zmq::Helper::get_subscriber_socket(){
    auto socket = std::unique_ptr<zmq::socket_t>(new zmq::socket_t(context_, ZMQ_SUB));
    socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
    return std::move(socket);
};

int zmq::Helper::poll_socket(zmq::socket_t& socket, std::chrono::milliseconds timeout){
    return zmq::poll({{socket, 0, ZMQ_POLLIN, 0}}, timeout.count());
}