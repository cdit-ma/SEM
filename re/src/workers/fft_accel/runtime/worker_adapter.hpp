//
// Created by cdit-ma on 31/8/20.
//

#ifndef SEM_WORKER_ADAPTER_HPP
#define SEM_WORKER_ADAPTER_HPP

//#include <network/adapter.hpp>
#include "adapter.hpp"

#include "worker.h"

namespace sem::fft_accel::runtime {

    class worker_adapter :
            public adapter,
            public Worker,
            public std::enable_shared_from_this<worker_adapter> {
    public:
        /**
         * An FFT Acceleration worker responsible for transmitting requests to an FFT Acceleration Engine,
         * as well as returning the results it has received in response to an FFT execution request.
         * @param container
         * @param inst_name
         */
        worker_adapter(const BehaviourContainer &container, const std::string &inst_name);

        ~worker_adapter() final = default;

        const std::string &get_version() const final;

        Result<void> set_network_adapter(std::weak_ptr<network::adapter> network_adapter) final;

        Result<void> receive_processed_fft(std::vector<float> data) final;

        /**
         * A grouping of constant component attribute names
         */
        struct AttrNames {
            /// The qualified endpoint (eg ip address + port) for the FFT Acceleration Engine
            constexpr static std::string_view accelerator_endpoint{"accelerator_endpoint"};
        };

    protected:
        //void HandleActivate() override;

        void HandleConfigure() override {
            auto endpoint = GetAttribute(std::string(AttrNames::accelerator_endpoint)).lock();
            if (endpoint == nullptr) {
                throw std::runtime_error(
                        "No accelerator_endpoint attribute found during FFT Acceleration Worker configuration.");
            }


            BehaviourContainer::HandleConfigure();
        };

    private:
        std::shared_ptr<network::adapter> adapter_;
    };


}


#endif //SEM_WORKER_ADAPTER_HPP
