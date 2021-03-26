//
// Created by Jackson Michael on 5/3/21.
//

#ifndef SEM_TCP_RECEIVER_H
#define SEM_TCP_RECEIVER_H

#include <utility>
#include <vector>
#include <future>

#include "boost/asio.hpp"

#include "tcp_stream_listener.hpp"
#include "data_stream.hpp"


namespace sem::logging::netdata {

    class tcp_receiver {
    public:
        /**
         * Begin listening for incoming TCP streams on the provided port
         * @param port
         */
        explicit tcp_receiver(uint16_t port);
        ~tcp_receiver();

        /**
         * Register that the provided listener should be notified of new lines received by a
         * tcp stream
         * @param listener
         */
        void register_stream_listener(const std::weak_ptr<tcp_stream_listener> &listener);

        /**
         * Blocks until the server stops running (ie when the internal service thread handling network
         * driver operations has finished)
         */
        void wait();

    private:
        std::vector<std::weak_ptr<tcp_stream_listener>> listeners_;

        std::thread service_thread_;

        // Order of initialisation of boost member classes matters;
        // work guard and acceptor are dependant on IO service so must come second
        boost::asio::io_service io_service_;
        boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_work_guard_;

        boost::asio::ip::tcp::acceptor acceptor_;

        std::vector<std::shared_ptr<data_stream>> data_streams_;

        void accept_connection();

        void register_data_stream(std::unique_ptr<boost::asio::ip::tcp::iostream> socket);

    };

}


#endif //SEM_TCP_RECEIVER_H
