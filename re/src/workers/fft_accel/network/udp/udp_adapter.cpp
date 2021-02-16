//
// Created by Jackson Michael on 19/8/20.
//

#include "udp_adapter.hpp"

using namespace std::string_literals;
using namespace sem;
using namespace sem::fft_accel::network;

using namespace boost::asio;

template<>
udp_adapter<float>::udp_adapter(sem::types::SocketAddress accel_engine_addr) :
        io_work_guard_(boost::asio::make_work_guard(io_service_)),
        udp_socket_(io_service_),
        fft_engine_(ip::make_address_v4(accel_engine_addr.ip().octets()),
                    accel_engine_addr.port()) {

    udp_socket_.open(ip::udp::v4());
    udp_socket_.bind(ip::udp::endpoint(ip::address_v4::any(), 0));

    schedule_listen();

    listen_thread_ = std::thread( [this]() {
        try {
            io_service_.run();
        } catch (const std::exception &ex) {
            std::cout << "An exception occurred on the UDP Adapter thread running the io_service:\n" << ex.what()
                      << std::endl;
        }
    });
}

template<>
udp_adapter<float>::~udp_adapter() {
    try {
        udp_socket_.cancel();
    } catch (const std::exception &ex) {
        std::cerr << "exception thrown when cancelling socket in  UDP adapter destructor: " << ex.what() << std::endl;
    }

    try{
        io_work_guard_.reset();
    } catch (const std::exception &ex) {
        std::cerr << "exception thrown when resetting work guard in UDP adapter destructor: " << ex.what() << std::endl;
    }

    try{
        io_service_.stop();
    } catch (const std::exception &ex) {
        std::cerr << "exception thrown when stopping IO service in UDP adapter destructor: " << ex.what() << std::endl;
    }

    try{
        listen_thread_.join();
    } catch (const std::exception &ex) {
        std::cerr << "exception thrown while waiting on listen thread in UDP adapter destructor: " << ex.what() << std::endl;
    }
}

template<>
Result<void> udp_adapter<float>::send(data_packet data) {
    try {
        auto &&bytes = data.bytes();
        // Address dereferencing shuffle required for MSVC compatibility
        auto buffer = boost::asio::buffer(&*data.bytes().begin(), data.bytes().size());
        auto bytes_sent = udp_socket_.send_to(buffer, fft_engine_);

        if (bytes_sent != data.bytes().size()) {
            return ErrorResult("UDP send(): number of bytes sent doesn't match input data");
        }

        return {};
    } catch (const std::exception &ex) {
        return ErrorResult("UDP send() failed with exception:\n"s + ex.what());
    } catch (...) {
        return ErrorResult("UDP send() failed; non-exception type was thrown");
    }
}

template<>
Result<void> udp_adapter<float>::register_listener(std::weak_ptr<response_packet_listener> listener) {
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
        return ErrorResult("Error when attempting to retrieve UDP adapter's bound port: "s + ex.what());
    }
}

template<typename SampleType>
void udp_adapter<SampleType>::schedule_listen() {
    auto &&bytes = recv_packet_.bytes();

    // Address dereferencing shuffle required for MSVC compatibility
    auto recv_buffer = buffer(&*bytes.begin(), bytes.size());

    udp_socket_.async_receive_from(
            recv_buffer, recv_sender_,
            [this](boost::system::error_code error, size_t bytes_transferred) {
                switch (error.value()) {
                    case boost::system::errc::success: {
                        if (bytes_transferred != recv_packet_.bytes().size()) {
                            std::cerr << "Received the wrong amount of bytes yet packet receive still reported success"
                                      << std::endl;
                        }

                        handle_fft_result(recv_packet_);
                        break;
                    }
                    case boost::system::errc::operation_canceled:
                        io_service_.stop();
                        return;
                    default:
                        handle_error_result(error);
                        return;
                }

                schedule_listen();
            });
}