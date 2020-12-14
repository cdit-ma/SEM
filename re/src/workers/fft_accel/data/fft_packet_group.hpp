//
// Created by Jackson Michael on 25/11/20.
//

#ifndef SEM_FFT_ACCEL_PACKET_GROUP_HPP
#define SEM_FFT_ACCEL_PACKET_GROUP_HPP

#include "data/fft_data.hpp"

#include <vector>

namespace sem::fft_accel::data {

    /**
     * A group of packets that represent a single FFT execution request to be sent to an FFT Acceleration Engine.
     * The request id will be the same for each packet in the group, but must be unique for each packet group.
     * The ordering of the packets within the packet group is determined by the sequence number of each packet.
     * @tparam SampleType
     */
    template<typename SampleType>
    class fft_packet_group {
    public:
        using request_id_type = data::data_request_id;
        using packet_type = data::fft_data_packet<SampleType>;

        /**
         * Construct a group of packets for a given request. The input data will be split amongst the created packets
         * @param fft_data
         * @param request_id
         */
        fft_packet_group(const std::vector<SampleType> &fft_data, request_id_type request_id);

#ifdef _WIN32
        [[deprecated("Default constructor only exists for compatibility with MSVC 2019 std::future")]]
        fft_packet_group(){};
#endif

        sem::Result<void> update_packet_with_response(data::fft_data_packet<SampleType> response);

        [[nodiscard]] constexpr std::vector<packet_type> &packets() { return packets_; }

        [[nodiscard]] constexpr request_id_type request_id() const { return request_id_; }

        [[nodiscard]] constexpr size_t remaining_packets() const { return num_remaining_packets_; };

        [[nodiscard]] std::vector<SampleType> to_vector() const;

    private:
        request_id_type request_id_;
        std::vector<packet_type> packets_;
        std::vector<bool> response_received_list_;
        size_t num_remaining_packets_;
    };

    template<typename SampleType>
    fft_packet_group<SampleType>::fft_packet_group(const std::vector<SampleType> &fft_data,
                                                   request_id_type request_id) :
            request_id_(request_id) {
        if (fft_data.empty()) {
            throw std::invalid_argument("Error: cannot construct fft packet group from empty vector");
        }

        // Use ceiling division (ie round fractions up rather than down
        // Subtract 1 to number of packets to ensure the "exactly divisible" edge case also gets rounded down
        size_t num_packets = (fft_data.size() - 1) / packet_type::max_elements + 1;

        response_received_list_ = std::vector<bool>(num_packets, false);
        num_remaining_packets_ = num_packets;

        packets_.reserve(num_packets);
        for (size_t cur_index = 0; cur_index < num_packets; cur_index++) {
            size_t segment_start = cur_index * packet_type::max_elements;

            // End is exclusive (ie not the last element intended to be in the payload data segment, but the one after)
            size_t segment_end = (cur_index + 1) * packet_type::max_elements;
            segment_end = std::min(segment_end, fft_data.size());

            data::vector_range<SampleType> data_segment{fft_data, segment_start, segment_end};

            packets_.emplace_back(data_segment, request_id, cur_index);
        }
    }

    template<typename SampleType>
    sem::Result<void>
    fft_packet_group<SampleType>::update_packet_with_response(data::fft_data_packet<SampleType> response) {
        auto seq_num = response.sequence_number();
        try {
            if (response_received_list_.at(seq_num) == true) {
                return ErrorResult("Attempting to update packet when a response had already been received...");
            }

            packets_.at(response.sequence_number()) = response;
            response_received_list_.at(response.sequence_number()) = true;
            num_remaining_packets_--;

            return {};
        } catch (const std::exception &ex) {
            return ErrorResult("Exception occurred while updating packet group with response packet (sequence number " +
                               std::to_string(seq_num) + "):\n" + ex.what());
        }
    }

    template<typename SampleType>
    std::vector<SampleType> fft_packet_group<SampleType>::to_vector() const {
        std::vector<SampleType> output_vec;
        output_vec.reserve(packet_type::max_elements * packets_.size());
        for (const auto &packet : packets_) {
            auto &&packet_vec = packet.payload_data();
            output_vec.insert(output_vec.end(), packet_vec.begin(), packet_vec.end());
        }
        return output_vec;
    }
}

#endif //SEM_FFT_PACKET_GROUP_HPP
