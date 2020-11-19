//
// Created by Jackson Michael on 8/9/20.
//


#include "fft_data.hpp"

using namespace sem::fft_accel::data;

template <packet_type PacketType>
void fill_control_from(control_data& data, const byte_span& bytes);

template <>
void fill_control_from<packet_type::data>(control_data& data, const byte_span& bytes) {
    std::byte frag_sequence_num_byte = bytes[1];
    data.packet_sequence_num_ = static_cast<uint8_t>(frag_sequence_num_byte);

    std::byte fft_request_id = bytes[2];
    data.fft_request_id_ = static_cast<uint8_t>(fft_request_id);

    std::byte control_state_register_byte = bytes[3];
    data.control_state_register_ = static_cast<uint8_t>(control_state_register_byte);

    auto fft_length_short = bytes.at<uint16_t>(4);
    uint16_t fft_length = ntohs(fft_length_short);
}

template <>
fft_data_packet<float>::fft_data_packet(const SerializedPacket &serialized_data) {
    auto&& bytes = serialized_data.bytes();

    packet_type type = packet_type_from_byte(bytes[0]);
    if (type != packet_type::data) {
        throw std::runtime_error("Attempting to construct data packet from non-data packet bytestream");
    }


}

//fft_data::~fft_data() = default;