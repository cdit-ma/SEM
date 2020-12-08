//
// Created by Jackson Michael on 2/12/20.
//

#ifndef SEM_UDP_PACKET_LOOPBACK_HPP
#define SEM_UDP_PACKET_LOOPBACK_HPP

#include "boost/asio.hpp"

#include "data/fft_data.hpp"

namespace sem::fft_accel::test {

    class udp_packet_loopback {
    public:
        udp_packet_loopback();
        ~udp_packet_loopback();

        [[nodiscard]] uint16_t get_port() const;

        void schedule_listen();

    private:

        std::future<void> listen_thread_;

        // Order of initialisation of boost member classes matters; socket is dependant on IO service so must come second
        boost::asio::io_service io_service_;

        boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_work_guard_;

        boost::asio::ip::udp::endpoint sender_endpoint_{};
        boost::asio::ip::udp::socket udp_socket_;

        data::fft_data_packet<float>::SerializedPacket recv_packet_;
    };

}

#endif //SEM_UDP_PACKET_LOOPBACK_HPP
