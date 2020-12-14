//
// Created by Jackson Michael on 19/8/20.
//

#ifndef SEM_NETWORK_ADAPTER_HPP
#define SEM_NETWORK_ADAPTER_HPP

#include "result.hpp"
#include "data/fft_data.hpp"

#include <memory>

namespace sem::fft_accel::network {

    /**
     * A class that implements the response_packet_listener interface must also implement the receive_response_packet
     * function. This allow these implementing classes to be registered with objects that will produce
     * fft results.
     */
    class response_packet_listener {
    public:
        using data_packet = data::fft_data_packet<float>::SerializedPacket;

        virtual ~response_packet_listener() = 0;

        /**
         * The receive_response_packet function will be called by other other classes when they have data packets
         * to pass to the fft_result_listener in question
         * @param data The serialized response packet
         * @return void result if the listener was able to successfully receive the data packet, otherwise returns an
         *     ErrorResult with a description of the error encountered.
         */
        virtual Result<void> receive_response_packet(data_packet data) = 0;
    };

    /**
     * Interface for FFT Acceleration network adapters that will send/receive FFT network traffic to an FAE.
     * An Adapter can exist in either an active or inactive state. Implementations are not required to guarantee that an
     * Adapter is in either state immediately after being constructed.
     * When in the inactive state, any operations related to sending and receiving messages is not guaranteed to function
     * as would normally happen in the active state, and should instead indicate that an error has occurred.
     */
    class adapter {
    public:
        using data_packet = data::fft_data_packet<float>::SerializedPacket;

        virtual ~adapter() = 0;

        virtual Result<void> send(data_packet data) = 0;

        virtual Result<void> register_listener(std::weak_ptr<response_packet_listener> listener) = 0;

        [[nodiscard]]
        virtual Result<uint16_t> get_bound_port() const = 0;
    };


}

#endif //SEM_NETWORK_ADAPTER_HPP
