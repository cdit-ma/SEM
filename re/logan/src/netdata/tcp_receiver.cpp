//
// Created by Jackson Michael on 5/3/21.
//

#include "tcp_receiver.h"

#include <typeinfo>

using namespace sem::logging::netdata;

using namespace boost::asio;

tcp_receiver::tcp_receiver(uint16_t port) :
        io_service_(),
        acceptor_(io_service_, ip::tcp::endpoint(ip::address(), port)) {
    service_thread_ = std::thread([&svc = io_service_](){
        svc.run();
    });
}

void tcp_receiver::register_chart_listener(sem::logging::netdata::json::chart_type key,
                                           const std::weak_ptr<chart_listener> &listener) {

    auto locked_listener = listener.lock();
    if (!locked_listener) {
        throw std::runtime_error("Error when registering chart listener; unable to lock listener");
    }

    listeners_.push_back(listener);
}

void tcp_receiver::accept_connection() {
    auto new_sock = sockets_.emplace_back();

    acceptor_.async_accept(*new_sock, [this, new_sock](boost::system::error_code ec){

        parse_socket_stream(new_sock);

        accept_connection();
    });
}

void tcp_receiver::parse_socket_stream(std::shared_ptr<ip::tcp::socket> socket) {
    //TODO: spawn a listen thread for each socket stream
}


