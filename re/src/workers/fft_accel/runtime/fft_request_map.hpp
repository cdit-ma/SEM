//
// Created by Jackson Michael on 17/11/20.
//

#ifndef SEM_FFT_REQUEST_MAP_HPP
#define SEM_FFT_REQUEST_MAP_HPP

#include "data/fft_data.hpp"

#include "result.hpp"

#include <unordered_map>
#include <future>

#include "data/fft_packet_group.hpp"

namespace sem::fft_accel::runtime {

    /**
     * Data grouping of the packet group associated with a given FFT request, along with a
     * future that can be used to wait for the processed result of the given request
     * @tparam SampleType
     */
    template<typename SampleType>
    struct pending_request {
        using response_type = data::fft_packet_group<SampleType>;
        using request_type = data::fft_packet_group<SampleType>;

        pending_request(std::future<response_type> unfulfilled_future, request_type packets) :
                packet_group(packets),
                future(std::move(unfulfilled_future)) {};
        request_type packet_group;
        std::future<response_type> future;
    };

    template<typename SampleType>
    struct fft_result {
        fft_result(data::data_request_id result_id, std::vector<SampleType> result_data) :
                id(result_id), data(result_data) {};
        data::data_request_id id;
        std::vector<SampleType> data;
    };

    /**
     * This class is a datastore for all of the current requests for which a response has yet to be received.
     * @tparam SampleType
     */
    template<typename SampleType>
    class fft_request_map {
    public:
        using request_id_type = data::data_request_id;
        using fft_request_type = data::fft_packet_group<SampleType>;
        using packet_type = typename fft_request_type::packet_type;

        /**
         * Construct an empty map, which will assign FFT request IDs beginning from 0
         */
        fft_request_map() : id_tracker_{0} {};

        /**
         * Constructs the FFT request (and the packets that it represents) required for an FFT request from the
         * provided data
         * @param fft_input_data The FFT samples to be used provided in interleaved-complex form
         * @return
         */
        Result<pending_request<SampleType>> construct_packets(const std::vector<SampleType> &fft_input_data);

        /**
         * When a new packet has been received in response to one that has been sent earlier this function should
         * be called to mark that it has been received for the appropriate response
         * @param received_packet
         * @return
         */
        Result<std::optional<fft_result<SampleType>>> update_pending_packets(packet_type received_packet);

    private:
        using packet_group_map = std::unordered_map<request_id_type, fft_request_type>;

        sem::Result<request_id_type> get_new_id();

        /// Request packet groups that have been created and allocated memory, but are yet to be sent
        packet_group_map staged_packet_groups_{};
        /// Keep track of the list of response promises that have yet to be fulfilled
        std::unordered_map<request_id_type, std::promise<fft_request_type>> unfulfilled_promises_;

        request_id_type id_tracker_;

        mutable std::mutex map_mutex_;
    };

    template<typename SampleType>
    sem::Result<pending_request<SampleType>>
    fft_request_map<SampleType>::construct_packets(const std::vector<SampleType> &fft_input_data) {
        std::lock_guard map_lock_guard(map_mutex_);
        try {
            auto new_id_result = get_new_id();
            if (new_id_result.is_error()) {
                return ErrorResult("Failed to generate a new ID when constructing a packet group: \n" +
                                   new_id_result.GetError().msg);
            }
            auto new_id = new_id_result.GetValue();

            // Create the new packet and return it
            auto[map_iter, successfully_inserted] = staged_packet_groups_.try_emplace(
                    new_id,
                    fft_input_data, new_id);
            if (!successfully_inserted) {
                return ErrorResult(
                        "fft_request_map failed to construct a packet group: unable to insert into staged packets map");
            }

            auto[promise_map_iter, promise_insert_success] = unfulfilled_promises_.try_emplace(new_id);
            if (!promise_insert_success) {
                return ErrorResult(
                        "fft_request_map failed to promise for unfulfilled request: unable to insert into staged packet map");
            }


            auto &&future = promise_map_iter->second.get_future();
            auto &&packet_group = map_iter->second;
            return {pending_request<SampleType>{std::move(future), packet_group}};


        } catch (const std::exception &ex) {
            return ErrorResult(
                    std::string("fft_request_map failed to construct a packet; An exception occurred: ")
                    + ex.what());
        }
    }

    template<typename SampleType>
    sem::Result<typename fft_request_map<SampleType>::request_id_type>
    fft_request_map<SampleType>::get_new_id() {
        // Cycle through available id values until a valid value is found
        // OR until we've cycled through the entire range of values
        request_id_type new_id;
        unsigned int attempts = 0;
        do {
            new_id = id_tracker_++;
            attempts++;
            if (attempts >= std::numeric_limits<request_id_type>::max()) {
                return ErrorResult(
                        "unable to find a free request_id; max attempts reached");
            }
        } while (staged_packet_groups_.count(new_id) != 0);

        return {new_id};
    }

    template<typename SampleType>
    Result<std::optional<fft_result<SampleType>>>
    fft_request_map<SampleType>::update_pending_packets(fft_request_map::packet_type received_packet) {
        auto request_id = received_packet.request_id();
        try {
            auto &&request = staged_packet_groups_.at(request_id);

            auto update_result = request.update_packet_with_response(received_packet);
            if (update_result.is_error()) {
                return ErrorResult(
                        "Couldn't update data for request id " + std::to_string(request_id) + ": " +
                        update_result.GetError().msg);
            }

            if (request.remaining_packets() == 0) {
                auto fulfilled_request = std::move(staged_packet_groups_.at(request_id));
                staged_packet_groups_.erase(request_id);
                unfulfilled_promises_.at(request_id).set_value(fulfilled_request);
                return {std::optional<fft_result<SampleType>>({request_id, fulfilled_request.to_vector()})};
            } else {
                return {std::optional<fft_result<SampleType>>()};
            }
        } catch (const std::exception &ex) {
            return ErrorResult("Exception while updating packet with request ID " + std::to_string(request_id) +
                               " with new packet data:\n" + ex.what());
        }
    }

}


#endif //SEM_FFT_REQUEST_MAP_HPP
