//
// Created by Jackson Michael on 2/12/20.
//

#include "udp_packet_loopback.hpp"

#include <iostream>

using namespace sem::fft_accel;
using namespace boost::asio;

test::udp_packet_loopback::udp_packet_loopback() :
        io_work_guard_(boost::asio::make_work_guard(io_service_)),
        udp_socket_(io_service_) {

    udp_socket_.open(ip::udp::v4());
    udp_socket_.bind(
            ip::udp::endpoint(ip::address_v4::loopback(), 0));

    schedule_listen();

    listen_thread_ = std::async([this]() {
        try {
            io_service_.run();
        } catch (const std::exception &ex) {
            std::cerr << "Exception during udp_loopback run: " << ex.what() << std::endl;
        }
    });
}

test::udp_packet_loopback::~udp_packet_loopback() {
    try {
        udp_socket_.cancel();
        io_work_guard_.reset();
        io_service_.stop();
        listen_thread_.wait();
    } catch (const std::exception& ex) {
        std::cerr << "exception thrown during UDP packet loopback destructor: " << ex.what() << std::endl;
    }
}

uint16_t test::udp_packet_loopback::get_port() const {
    return udp_socket_.local_endpoint().port();
}

void test::udp_packet_loopback::schedule_listen() {
    // Address dereferencing shuffle required for MSVC compatibility
    auto recv_buffer = buffer(&*recv_packet_.bytes().begin(), recv_packet_.bytes().size());

    auto response_handler = [this](boost::system::error_code error, size_t bytes_transferred) {
            switch (error.value()) {
                case boost::system::errc::operation_canceled:
                    std::cerr << "UDP loopback was cancelled" << std::endl;
                    return;
                case boost::system::errc::success: {
                    auto&& bytes = recv_packet_.bytes();
                    // Address dereferencing shuffle required for MSVC compatibility
                    auto send_buffer = buffer(&*bytes.begin(), bytes.size());
                    udp_socket_.send_to(send_buffer, sender_endpoint_);
                    schedule_listen();
                    return;
                }
                default:
                    std::cerr << "An error occurred during udp loopback listen loop:\n" << error << std::endl;
                    return;
            }
        };

    try {
        udp_socket_.async_receive_from(recv_buffer, sender_endpoint_, response_handler);
    } catch (const std::exception &ex) {
        std::cerr << "An error occurred during async_receive_from:\n" << ex.what() << std::endl;
    }
}