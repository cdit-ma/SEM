//
// Created by Jackson Michael on 25/11/20.
//

#include "packet_headers.hpp"

#include "boost/endian/conversion.hpp"

using namespace sem::fft_accel::data;

using namespace boost::endian;

data_packet_header::data_packet_header(const byte_span &bytes) :
        packet_sequence_num_(static_cast<uint8_t>(bytes[1])),
        fft_request_id_(static_cast<uint8_t>(bytes[2])),
        control_state_register_(static_cast<uint8_t>(bytes[3])), // Unused
        fft_size_(big_to_native(bytes.at<uint16_t>(4))) {
}

data_packet_header::data_packet_header(data_request_id request_id, uint8_t sequence_num, uint16_t fft_size) :
        fft_request_id_(request_id),
        packet_sequence_num_(sequence_num),
        fft_size_(fft_size),
        control_state_register_(0) // CSR is unused, default to 0
{

}
