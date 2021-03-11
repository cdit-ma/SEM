//
// Created by Jackson Michael on 5/3/21.
//

#ifndef SEM_TCP_RECEIVER_H
#define SEM_TCP_RECEIVER_H

#include <vector>
#include <future>

#include "boost/asio.hpp"

#include "json_info.hpp"
#include "chart_listener.hpp"


namespace sem::logging::netdata {

    class tcp_receiver {
    public:
        explicit tcp_receiver(uint16_t port);

        void register_chart_listener(json::chart_type key, const std::weak_ptr<chart_listener>& listener);

    private:
        std::vector<std::weak_ptr<chart_listener>> listeners_;

        std::thread service_thread_;

        // Order of initialisation of boost member classes matters; socket is dependant on IO service so must come second
        boost::asio::io_service io_service_;

        //boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_work_guard_;

        //boost::asio::ip::udp::endpoint sender_endpoint_{};
        boost::asio::ip::tcp::acceptor acceptor_;

        std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> sockets_;

        void accept_connection();

        void parse_socket_stream(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    };

}


#endif //SEM_TCP_RECEIVER_H
