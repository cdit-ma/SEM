//
// Created by Jackson Michael on 16/3/21.
//

#ifndef SEM_DATA_STREAM_HPP
#define SEM_DATA_STREAM_HPP

#include "boost/asio.hpp"

#include "tcp_stream_listener.hpp"

#include <thread>
#include <vector>

#include <iostream>

namespace sem::logging::netdata {

    class data_stream {
        using tcp_socket = boost::asio::ip::tcp::iostream;

    public:
        explicit data_stream(std::unique_ptr<tcp_socket> sock) :
                socket_(std::move(sock)),
                listen_thread_(&data_stream::listen_loop, this) {
        }

        ~data_stream() {
            socket_->close();
            listen_thread_.join();
        }

        void register_listener(std::weak_ptr<tcp_stream_listener> listener) {
            listeners_.push_back(std::move(listener));
        }

    private:
        std::unique_ptr<tcp_socket> socket_;
        std::thread listen_thread_;

        std::vector<std::weak_ptr<tcp_stream_listener>> listeners_;


        void listen_loop() {
            while (socket_->good()) {
                std::string new_line;
                std::getline(*socket_, new_line);

                if (socket_->eof()) {
                    break;
                }
                notify_line_received(new_line);
            }
        }

        void notify_line_received(const std::string& line) {
            for (const auto& listener_ref : listeners_) {
                if (auto locked_ref = listener_ref.lock()) {
                    locked_ref->receive_line(line);
                }
            }
        }
    };

}

#endif //SEM_DATA_STREAM_HPP
