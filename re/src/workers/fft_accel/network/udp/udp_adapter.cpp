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
    const size_t num_floats = data.size() / sizeof(float);

    return {std::vector<float>(buffer_as_floats, (buffer_as_floats + num_floats))};
}

template<>
udp_adapter<float>::udp_adapter(re::types::SocketAddress accel_engine_addr) :
        io_work_guard_(boost::asio::make_work_guard(io_service_)),
        udp_socket_(io_service_),
        fft_engine_(ip::make_address_v4(accel_engine_addr.ip().octets()),
                    accel_engine_addr.port()),
        listen_vec_(8),
        listen_buffer_(buffer(listen_vec_, 8*sizeof(float))){

    udp_socket_.open(ip::udp::v4());
    udp_socket_.bind(ip::udp::endpoint(ip::address_v4::loopback(), 0));

    udp_socket_.async_receive_from(
            listen_buffer_, fft_engine_,
            [this](boost::system::error_code error, size_t bytes_transferred){
                if (error != boost::system::errc::success) {
                    handle_error_result(error);
                } else {
                    handle_fft_result(listen_vec_);
                }

            });

    //listen_thread_ = std::async([this]()  {
    listen_thread_ = boost::thread([this]()  {
        try {
            io_service_.run();
        } catch (const std::exception& ex) {
            std::cout << "Exception during run: " << std::endl;
        }
    });
}

template<>
udp_adapter<float>::~udp_adapter() {
    udp_socket_.cancel();
    io_work_guard_.reset();
    io_service_.stop();
    //listen_thread_->wait();
    listen_thread_.join();
}

template<>
Result<void> udp_adapter<float>::send(fft_data data) {
    try {
        const auto &buffer = serialize<const_buffer>(data).GetValue();
        auto bytes_sent = udp_socket_.send_to(buffer, fft_engine_);

        if (bytes_sent != data.size() * sizeof(float)) {
            return ErrorResult("UDP send(): number of bytes sent doesn't match input data");
        }

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


template<>
Result<uint16_t> udp_adapter<float>::get_bound_port() const {

    try {
        return {udp_socket_.local_endpoint().port()};
    } catch (const std::exception &ex) {
        return {ErrorResult("Error when attempting to retrieve UDP adapter's bound port: "s + ex.what())};
    }
}