//
// Created by Jackson Michael on 19/8/20.
//

#ifndef SEM_UDP_ADAPTER_HPP
#define SEM_UDP_ADAPTER_HPP

#include "network/adapter.hpp"

#include "socketaddress.hpp"
#include "boost/asio.hpp"
#include <vector>

/*namespace sem::fft_accel::data {

    template<typename SampleType>
    class data::serializable_fft_data<SampleType, boost::asio::const_buffer> {
    public:

        serializable_fft_data(std::vector<SampleType> data) : fft_data_(data) {};

        serializable_fft_data(boost::asio::const_buffer &data) :
                fft_data_(data) {
        };

        boost::asio::const_buffer serialize() {
            return {fft_data_.data(), fft_data_.size() * sizeof(SampleType)};
        }
    private:
        std::vector<SampleType> fft_data_;
    };
}*/


namespace sem::fft_accel::network {

    template<>
    Result<boost::asio::const_buffer> serialize(const std::vector<float> &data);

    template<>
    Result<std::vector<float>> deserialize(const boost::asio::const_buffer &data);

    template<typename SampleType>
    class udp_adapter final : public adapter {
        using fft_data = std::vector<SampleType>;

    public:
        explicit udp_adapter(re::types::SocketAddress accel_engine_addr);

        ~udp_adapter() final;

        Result<void> send(fft_data data) final;

        Result<void> register_listener(std::weak_ptr<fft_result_listener> listener) final;

        [[nodiscard]]
        Result<uint16_t> get_bound_port() const final;

    private:
        /**
         * Passes the fft result that has been received over the network to all of the registered listeners
         * @param fft_vector
         */
        void handle_fft_result(const std::vector<SampleType> &fft_vector) {
            for (auto &weak_ptr : listeners_) {
                auto listener = weak_ptr.lock();
                listener->receive_processed_fft(fft_vector);
            }
        }

        void handle_error_result(boost::system::error_code ec) {
            // TODO: Hook into a log_error function in the containing class
            std::cerr << "An error occurred when reading from a UDP adapter socket: " << ec.message() << std::endl;
        };

        void schedule_listen();

        std::vector<std::weak_ptr<fft_result_listener>> listeners_;

        std::optional<std::future<void>> listen_thread_;

        // Order of initialisation of boost member classes matters; socket is dependant on IO service so must come second
        boost::asio::io_service io_service_;

        boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_work_guard_;

        boost::asio::ip::udp::endpoint fft_engine_;
        boost::asio::ip::udp::socket udp_socket_;

        std::vector<float> listen_vec_;
        boost::asio::mutable_buffer listen_buffer_;
    };

}
#endif //SEM_UDP_ADAPTER_HPP
