//
// Created by Jackson Michael on 8/9/20.
//

#include "fft_data.hpp"

using namespace sem::fft_accel::data;

template<>
fft_data_packet<float>::fft_data_packet(SerializedPacket &serialized_data) :
        header_data_(serialized_data.bytes().subspan(0,6)) {
    auto &&bytes = serialized_data.bytes();

    // TODO: Thoroughly investigate endianness of deserialized payload
    size_t payload_byte_length = (bytes.size() - 6);
    fft_data_.resize(payload_byte_length / sizeof(float));
    memcpy(&(*fft_data_.begin()), &(*bytes.subspan(6).begin()), payload_byte_length);
}

template<>
uint8_t fft_data_packet<float>::sequence_number() const { return header_data_.sequence_number(); };

template<>
uint8_t fft_data_packet<float>::request_id() const { return header_data_.request_id(); };

template<>
const std::vector<float> &fft_data_packet<float>::payload_data() const { return fft_data_; };
