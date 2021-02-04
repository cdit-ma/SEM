//
// Created by Jackson Michael on 8/9/20.
//

#include "fft_data.hpp"

using namespace sem::fft_accel::data;

template<>
fft_data_packet<float>::fft_data_packet(SerializedPacket &serialized_data) :
        header_data_(serialized_data.bytes().subspan(0, SerializedPacket::header_byte_length)) {
    auto &&bytes = serialized_data.bytes();

    // Create a span from the remaining bytes after the header data and copy it into the vector
    // TODO: Thoroughly investigate endianness of deserialized payload
    auto data_span = bytes.subspan(SerializedPacket::header_byte_length);
    fft_data_.resize(data_span.size() / sizeof(float));
    memcpy(&(*fft_data_.begin()), &(*data_span.begin()), data_span.size());
}

template<>
uint8_t fft_data_packet<float>::sequence_number() const { return header_data_.sequence_number(); };

template<>
uint8_t fft_data_packet<float>::request_id() const { return header_data_.request_id(); };

template<>
const std::vector<float> &fft_data_packet<float>::payload_data() const { return fft_data_; };
