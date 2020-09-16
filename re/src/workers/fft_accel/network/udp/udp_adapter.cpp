//
// Created by Jackson Michael on 19/8/20.
//

#include "udp_adapter.hpp"

using namespace std::string_literals;
using namespace sem;
using namespace sem::fft_accel::network;

using namespace boost::asio;


template<>
Result<boost::asio::const_buffer> sem::fft_accel::network::serialize(const std::vector<float> &data) {
    // Note: Using 'raw' serialization of bytes used to represent floats - probably unsafe on some platforms
    const auto *data_floats_as_bytes = reinterpret_cast<const std::byte *>(data.data());

    return {const_buffer(data_floats_as_bytes, data.size() * sizeof(float))};
}

template<>
Result<std::vector<float>> sem::fft_accel::network::deserialize(const boost::asio::const_buffer &data) {
    // Ensure that we don't have left over bytes (ie half a float worth of bytes on the end)
    if (data.size() % sizeof(float) != 0) {
        return {ErrorResult(
                "Invalid boost::asio::const_buffer length - number of bytes isn't evenly divided by number of floats")};
    }

    const auto *buffer_as_floats = reinterpret_cast<const float *>(data.data());
    const size_t num_floats = data.size()/sizeof(float);

    return {std::vector<float>( buffer_as_floats, (buffer_as_floats+num_floats))};
}

template<>
udp_adapter<float>::udp_adapter(re::types::SocketAddress accel_engine_addr) :
        udp_socket_(io_service_),
        fft_engine_(ip::make_address_v4(accel_engine_addr.ip().octets()), accel_engine_addr.port()) {
    udp_socket_.open(ip::udp::v4());
}

template<>
Result<void> udp_adapter<float>::send(fft_data data) {
    try {
        const auto& buffer = serialize<const_buffer>(data).GetValue();
        udp_socket_.send_to(buffer, fft_engine_);
        return {};
    } catch (const std::exception &ex) {
        return ErrorResult("UDP send() failed with exception: "s + ex.what());
    } catch (...) {
        return ErrorResult("UDP send() failed; non-exception type was thrown");
    }
}

template<>
Result<void> udp_adapter<float>::register_listener(std::weak_ptr<fft_result_listener> listener) {
    try {
        listeners_.push_back(listener);
        return {};
    } catch (const std::exception &ex) {
        return ErrorResult(
                std::string("Unable to register FFT result listener: ") + ex.what()
        );
    }
}

/*
template<>
bool udp_adapter<float>::is_active() {

}
*/