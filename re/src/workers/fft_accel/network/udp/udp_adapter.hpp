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
    Result<boost::asio::const_buffer> serialize(const std::vector<float>& data);

    template<>
    Result<std::vector<float>> deserialize(const boost::asio::const_buffer& data);

    template<typename SampleType>
    class udp_adapter final : public adapter {
        using fft_data = std::vector<SampleType>;

    public:
        explicit udp_adapter(re::types::SocketAddress accel_engine_addr);

        ~udp_adapter() final = default;

        Result<void> send(fft_data data) final;

        Result<void> register_listener(std::weak_ptr<fft_result_listener> listener) final;

    private:
        /**
         * Send the bytes of input data that represent one full FFT operation
         * @param data
         * @param num_bytes
         * @return
         */
        Result<void> send_bytes(void *data, size_t num_bytes);

        Result<void> send_packet(void *data, size_t num_bytes);

        std::vector<std::weak_ptr<fft_result_listener>> listeners_;

        // Order of initialisation of boost member classes matters; socket is dependant on IO service so must come second
        boost::asio::io_service io_service_;
        boost::asio::ip::udp::socket udp_socket_;
        boost::asio::ip::udp::endpoint fft_engine_;
    };

}
#endif //SEM_UDP_ADAPTER_HPP
