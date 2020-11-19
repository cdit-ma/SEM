//
// Created by cdit-ma on 31/8/20.
//

#ifndef SEM_ADAPTER_IMPL_HPP
#define SEM_ADAPTER_IMPL_HPP

//#include <network/adapter.hpp>
#include "adapter.hpp"

#include "data/fft_data.hpp"
#include "fft_request_map.hpp"

#include "worker.h"

namespace sem::fft_accel::runtime {

    class adapter_impl final :
            public adapter,
            public std::enable_shared_from_this<adapter_impl> {
    public:
        /**
         * An FFT Acceleration worker responsible for transmitting requests to an FFT Acceleration Engine,
         * as well as returning the results it has received in response to an FFT execution request.
         * @param container
         * @param inst_name
         */
        adapter_impl();

        ~adapter_impl() final = default;

        Result<void> set_network_adapter(std::weak_ptr<network::adapter> network_adapter) final;

        Result<void> submit_fft_calculation(const std::vector<float>& data) final;

        Result<void> receive_processed_fft(std::vector<float> data) final;

    protected:
        //void HandleActivate() override;

    private:
        std::shared_ptr<network::adapter> adapter_;

        using request_id = uint8_t ;
        fft_request_map<float> pending_requests_;
    };


}


#endif //SEM_ADAPTER_IMPL_HPP
