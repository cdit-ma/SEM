//
// Created by Jackson Michael on 19/8/20.
//

#ifndef SEM_UDP_ADAPTER_HPP
#define SEM_UDP_ADAPTER_HPP

#include "network/adapter.hpp"

#include "socketaddress.hpp"
#include "boost/asio.hpp"
#include <vector>

namespace sem::fft_accel::network {


    template<typename SampleType>
    class udp_adapter final : public adapter {
    public:
        explicit udp_adapter(re::types::SocketAddress accel_engine_addr);

        ~udp_adapter() final;

        Result<void> send(data_packet data) final;

        Result<void> register_listener(std::weak_ptr<fft_result_listener> listener) final;

        [[nodiscard]]
        Result<uint16_t> get_bound_port() const final;

    private:
        /**
         * Passes the fft result that has been received over the network to all of the registered listeners
         * @param data The received packet in serialized form
         */
        void handle_fft_result(const data_packet &data) {
            for (auto &weak_ptr : listeners_) {

                auto listener = weak_ptr.lock();
                if (listener == nullptr) {
                    std::cerr << "Error: Attempting to send data to invalid fft_listener (nullptr)" << std::endl;
                    return;
                }

                auto result = listener->receive_processed_fft(data);
                if (result.is_error()) {
                    std::cerr << "An error occurred in a udp_adapter while handing results to an fft_listener:\n"
                              << result.GetError().msg << std::endl;
                }
            }
        }

        void handle_error_result(boost::system::error_code ec) {
            // TODO: Hook into a log_error function in the containing class
            std::cerr << "An error occurred when reading from a UDP adapter socket: " << ec.message() << std::endl;
        };

        /**
         * Signals that a new response handler should be added to the io_service task queue.
         * This must be called each time a response is handled as boost requires a new handler be submitted each time.
         */
        void schedule_listen();

        std::vector<std::weak_ptr<fft_result_listener>> listeners_;

        std::future<void> listen_thread_;

        // Order of initialisation of boost member classes matters; socket is dependant on IO service so must come second
        boost::asio::io_service io_service_;

        boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_work_guard_;

        boost::asio::ip::udp::endpoint fft_engine_;
        boost::asio::ip::udp::socket udp_socket_;

        boost::asio::ip::udp::endpoint recv_sender_;
        data_packet recv_packet_;
    };

}
#endif //SEM_UDP_ADAPTER_HPP
