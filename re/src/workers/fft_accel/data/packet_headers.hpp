//
// Created by Jackson Michael on 25/11/20.
//

#ifndef SEM_FFT_ACCEL_PACKET_HEADERS_HPP
#define SEM_FFT_ACCEL_PACKET_HEADERS_HPP

#include "byte_span.hpp"

namespace sem::fft_accel::data {
    using data_request_id = uint8_t;

    enum class packet_type : uint8_t {
        cmd_write,
        cmd_read,
        ack,
        data,
    };

    constexpr std::byte byte_encoding(packet_type type) {
        switch (type) {
            case packet_type::cmd_write:
                return static_cast<std::byte>(0xCDu);
            case packet_type::cmd_read:
                return static_cast<std::byte>(0xCEu);
            case packet_type::ack:
                return static_cast<std::byte>(0xACu);
            case packet_type::data:
                return static_cast<std::byte>(0xDAu);
        }
    }

    constexpr packet_type packet_type_from_byte(std::byte byte) {
        switch (static_cast<uint8_t>(byte)) {
            case 0xCDu:
                return packet_type::cmd_write;
            case 0xCEu:
                return packet_type::cmd_read;
            case 0xACu:
                return packet_type::ack;
            case 0xDAu:
                return packet_type::data;
            default:
                throw std::invalid_argument("Error attempting to parse packet type: invalid/unknown byte pattern");
        }
    }

    class data_packet_header {
    public:
        explicit data_packet_header(const byte_span& bytes);
        explicit data_packet_header(data_request_id request_id, uint8_t sequence_num, uint16_t fft_size);

        [[nodiscard]] data_request_id request_id() const {return fft_request_id_;};
        [[nodiscard]] uint8_t sequence_number() const {return packet_sequence_num_;};
        [[nodiscard]] uint16_t fft_size() const {return fft_size_;};
        [[deprecated]]
        [[nodiscard]] uint8_t csr() const {return control_state_register_;};

        constexpr static packet_type type = packet_type::data;
    private:
        uint8_t packet_sequence_num_;
        data_request_id fft_request_id_;
        uint8_t control_state_register_; // Unused, included for strict conformance with deprecated protocol specification
        uint16_t fft_size_;
    };
}

#endif //SEM_FFT_ACCEL_PACKET_HEADERS_HPP
