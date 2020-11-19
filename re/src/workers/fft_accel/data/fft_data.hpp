//
// Created by Jackson Michael on 8/9/20.
//

#ifndef SEM_FFT_DATA_HPP
#define SEM_FFT_DATA_HPP

#include <cstddef>
#include <vector>

#include "serialized.hpp"

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

struct control_data {
    packet_type type_;
    uint8_t packet_sequence_num_;
    data_request_id fft_request_id_;
    uint8_t control_state_register_;
    uint16_t fft_size_;
};

class fft_data {

};

/**
 *
 * @tparam SampleType The type of primitive data being used to store FFT sample values
 */
    template<typename SampleType>
    class fft_data_packet {
        using SerializedPacket = Serialized<fft_data_packet<SampleType>>;
    public:
        fft_data_packet(const std::vector<SampleType> &data, size_t packet_identifier) :
                fft_data_(data),
                fft_request_id_(packet_identifier) {};

        fft_data_packet(const SerializedPacket &serialized_data);

        SerializedPacket serialize();

    private:
        uint8_t packet_sequence_num_;
        uint8_t fft_request_id_;
        std::vector<SampleType> fft_data_;
    };

    template<typename SampleType, size_t NumElements>
    class serialized_fft_data : public Serialized<fft_data_packet<SampleType>> {
    public:
        constexpr static size_t byte_size_from_num_elements(size_t num_elements) {
            return num_elements * sizeof(SampleType) + 6;
        }

        using DeserializableType = fft_data_packet<SampleType>;
        using byte_array = std::array<std::byte, byte_size_from_num_elements(NumElements)>;
        explicit serialized_fft_data(byte_array data_buffer) : data_(data_buffer) {};

        serialized_fft_data(std::byte *data, size_t length) : data_(data, length) {};

        virtual DeserializableType deserialize() final;

        [[nodiscard]] virtual byte_span bytes() const final;

    private:
        byte_array data_;
    };

    template<typename SampleType, size_t NumElements>
    fft_data_packet<SampleType> serialized_fft_data<SampleType, NumElements>::deserialize() {
        return fft_data_packet<SampleType>(*this);
    }

    template<typename SampleType, size_t NumElements>
    byte_span serialized_fft_data<SampleType, NumElements>::bytes() const {
        return byte_span(data_);
    }


};

#endif //SEM_FFT_DATA_HPP
