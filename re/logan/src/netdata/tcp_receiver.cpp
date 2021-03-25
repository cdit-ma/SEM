//
// Created by Jackson Michael on 5/3/21.
//

#include "tcp_receiver.h"

using namespace sem::logging::netdata;

using namespace boost::asio::ip;

tcp_receiver::tcp_receiver(uint16_t port) :
        io_service_(),
        io_work_guard_(boost::asio::make_work_guard(io_service_)),
        acceptor_(io_service_, tcp::endpoint(address(), port)) {
    service_thread_ = std::thread([&svc = io_service_](){
        svc.run();
    });
    accept_connection();
}

tcp_receiver::~tcp_receiver() {
    acceptor_.close();
    data_streams_.clear();
    io_service_.stop();
    service_thread_.join();
}

void tcp_receiver::register_stream_listener(const std::weak_ptr<tcp_stream_listener> &listener) {

    auto locked_listener = listener.lock();
    if (!locked_listener) {
        throw std::runtime_error("Error when registering chart listener; unable to lock listener");
    }

    listeners_.push_back(listener);
    for (auto& stream : data_streams_) {
        stream->register_listener(locked_listener);
    }
}

void tcp_receiver::accept_connection() {
    acceptor_.async_accept([this](const boost::system::error_code& ec, tcp::socket sock) {
        register_data_stream(std::make_unique<tcp::iostream>(std::move(sock)));
        accept_connection();
    });
}

void tcp_receiver::register_data_stream(std::unique_ptr<tcp::iostream> iostream) {
    std::cout << "Registering new data stream from " << iostream->rdbuf()->remote_endpoint() << std::endl;
    auto new_stream = std::make_shared<data_stream>(std::move(iostream));
    for (const auto& listener : listeners_) {
        new_stream->register_listener(listener);
    }
    data_streams_.push_back(std::move(new_stream));
}

void tcp_receiver::wait() {
    service_thread_.join();
}