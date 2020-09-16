//
// Created by Jackson Michael on 19/8/20.
//

#ifndef SEM_NETWORK_ADAPTER_HPP
#define SEM_NETWORK_ADAPTER_HPP

#include "result.hpp"
#include "data/fft_data.hpp"

namespace sem::fft_accel::network {

    template <typename SerializedType, typename LocalType>
    Result<SerializedType> serialize(const LocalType& data);

    template <typename LocalType, typename SerializedType>
    Result<LocalType> deserialize(const SerializedType& data);

    class fft_result_listener {
    public:
        virtual ~fft_result_listener() = 0;

        virtual Result<void> receive_processed_fft(std::vector<float> data) = 0;
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

        virtual ~adapter() = 0;

        virtual Result<void> send(std::vector<float>) = 0;

        virtual Result<void> register_listener(std::weak_ptr<fft_result_listener> listener) = 0;

        /**
         * Used to check whether or not the adapter in question is in an active state. When not in an active state messages
         * will not be sent or received and will instead result in an error.
         * @return
         */
        //virtual bool is_active() = 0;
    };


}

#endif //SEM_NETWORK_ADAPTER_HPP
