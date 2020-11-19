//
// Created by cdit-ma on 17/11/20.
//

#ifndef SEM_FFT_REQUEST_MAP_HPP
#define SEM_FFT_REQUEST_MAP_HPP

#include "data/fft_data.hpp"

#include "result.hpp"

#include "unordered_map"

namespace sem::fft_accel {

    template<typename SampleType>
    class fft_request_map {
    public:
        using request_id_type = data::data_request_id;
        using packet_type = data::fft_data_packet<SampleType>;

        /**
         * Construct an empty map, which will begin assigning FFT request IDs beginning from 0
         */
        fft_request_map() : id_tracker_{0} {};

        sem::Result<packet_type> construct_packet(const std::vector<SampleType> &fft_input_data) {
            std::lock_guard map_lock_guard(map_mutex_);
            try {
                // Cycle through available id values until a valid value is found
                // OR until we've cycled through the entire range of values
                request_id_type new_id;
                unsigned int attempts = 0;
                do {
                    new_id = id_tracker_++;
                    attempts++;
                    if (attempts < std::numeric_limits<request_id_type>::max()) {
                        return ErrorResult(
                                "fft_request_map failed to construct a packet: unable to find a free request_id; max attempts reached");
                    }
                } while (staged_packets_.count(new_id) != 0);

                // Create the new packet and return it
                packet_type new_packet(fft_input_data, new_id);
                auto[map_iter, successfully_inserted] = staged_packets_.try_emplace(
                        new_id,
                        new_packet);
                if (!successfully_inserted) {
                    return ErrorResult(
                            "fft_request_map failed to construct a packet: unable to insert into staged packet map");
                }
                return map_iter->second;

            } catch (const std::exception &ex) {
                return ErrorResult(
                        std::string("fft_request_map failed to construct a packet; An exception occurred: ")
                        + ex.what());
            }
        };

        sem::Result<void> mark_as_sent(request_id_type id) {
            std::lock_guard map_lock_guard(map_mutex_);

            try {
                auto shifted_result = staged_packets_.extract(id);
                try {
                    response_pending_packets_.insert(shifted_result);
                } catch (const std::exception &ex) {
                    staged_packets_.insert(shifted_result);
                    ErrorResult(
                            std::string("fft_request_map failed to mark request as sent: "
                                        "failed to add to pending packets, returning to staged packet store: ")
                            + ex.what()
                    );
                }
            } catch (const std::exception &ex) {
                ErrorResult(std::string("fft_request_map failed to mark request as sent: "
                                        "") + ex.what());
            }
        };

        sem::Result<packet_type> pop_received_packet(request_id_type id) {
            std::lock_guard map_lock_guard(map_mutex_);
            try {
                auto map_iter = response_pending_packets_.erase(id);
                return map_iter->second;
            } catch (const std::exception& ex) {
                return ErrorResult(std::string("fft_request_map failed to pop a pending packet: ")+ ex.what());
            }
        };

    private:
        using data_packet_map = std::unordered_map<request_id_type, packet_type>;

        /// Packets that have been created and allocated memory, but are yet to be sent
        data_packet_map staged_packets_;

        /// Packets that have been sent but not yet received
        data_packet_map response_pending_packets_;

        request_id_type id_tracker_;

        mutable std::mutex map_mutex_;
    };

}


#endif //SEM_FFT_REQUEST_MAP_HPP
