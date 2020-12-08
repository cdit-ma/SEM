//
// Created by Jackson Michael on 31/8/20.
//

#ifndef SEM_ADAPTER_IMPL_HPP
#define SEM_ADAPTER_IMPL_HPP

#include "adapter.hpp"

#include "data/fft_data.hpp"
#include "fft_request_map.hpp"

#include "worker.h"

namespace sem::fft_accel::runtime {

    class adapter_impl final :
            public adapter,
            public std::enable_shared_from_this<adapter_impl> {
    public:
        adapter_impl();

        ~adapter_impl() final = default;

        Result<void> set_network_adapter(std::weak_ptr<network::adapter> network_adapter) final;

        Result<data::data_request_id> submit_fft_calculation(const std::vector<float>& data) final;

        Result<std::vector<float>> wait_on_request_completion(data::data_request_id id, re::types::Timeout timeout) final;

        Result<void> receive_processed_fft(data_packet data) final;

    private:
        using request_id = data::data_request_id;
        using future_type = std::future<data::fft_packet_group<float>>;

        std::shared_ptr<network::adapter> adapter_;

        fft_request_map<float> pending_requests_;
        std::unordered_map<request_id, future_type> response_futures_;
    };


}


#endif //SEM_ADAPTER_IMPL_HPP
