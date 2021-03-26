//
// Created by Jackson Michael on 22/3/21.
//

#ifndef SEM_DEVICE_SAMPLE_AGGREGATOR_HPP
#define SEM_DEVICE_SAMPLE_AGGREGATOR_HPP

#include <memory>
#include "chart_listener.hpp"

#include "systemevent.pb.h"

namespace sem::logging::netdata {

    class device_sample_aggregator : public chart_listener {
    public:
        /**
         * Handles newly received netdata nvidia_smi chart samples. Specifically, pulls out the gpu
         * and memory utilisation, as well as temperature
         * @param chart_data
         */
        void receive_chart_data(std::shared_ptr<const protobuf::chart> chart_data) override {
            auto &nvidia_smi = *chart_data;
            if (nvidia_smi.chart_type() != protobuf::chart::nvidia_smi) {
                std::string error_str{"Received an unexpected netdata chart when nvidia_smi was expected: "};
                throw std::invalid_argument(error_str + protobuf::chart::chart_type_enum_Name(nvidia_smi.chart_type()));
            }

            auto sample = get_sample(nvidia_smi.hostname(),
                                     nvidia_smi.chart_family(),
                                     nvidia_smi.timestamp());

            if (nvidia_smi.chart_context() == "nvidia_smi.gpu_utilization") {
                sample->set_processor_utilisation_perc(nvidia_smi.value());
            } else if (nvidia_smi.chart_context() == "nvidia_smi.mem_utilization") {
                sample->set_memory_utilisation_mib(nvidia_smi.value());
            } else if (nvidia_smi.chart_context() == "nvidia_smi.temperature") {
                sample->set_temperature_cel(nvidia_smi.value());
            } else {
                return;
            }
        }

        /**
         * Retrieves all of the current device metric samples for the current hostname, removing them from
         * the currently stored list within the chart_proto_converter
         * @param hostname
         * @return The uniquely owning pointer to the DeviceMetricSamples (empty if there are none for the
         * provided hostname)
         */
        std::unique_ptr<SystemEvent::DeviceMetricSamples> retrieve_device_metrics(const std::string& hostname) {
            try {
                return std::move(device_samples_.at(hostname));
            } catch (const std::out_of_range& ex) {
                return std::make_unique<SystemEvent::DeviceMetricSamples>();
            }
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<SystemEvent::DeviceMetricSamples>> device_samples_;

        /**
         * Gets a sample associated with the given hostname, device name and timestamp if one exists.
         * If not, create the sample, so long as the hostname matches.
         * @param hostname
         * @param device_name
         * @param timestamp
         * @return
         */
        SystemEvent::GPUMetricSample*
        get_sample(const std::string &hostname, const std::string &device_name, uint64_t timestamp) {
            auto &current_sample_group_ = device_samples_[hostname];
            if (!current_sample_group_) {
                current_sample_group_ = std::make_unique<SystemEvent::DeviceMetricSamples>();
                current_sample_group_->set_hostname(hostname);
            }

            for (auto&& gpu_sample : *current_sample_group_->mutable_gpu_samples()) {
                if (gpu_sample.device_name() == device_name &&
                    gpu_sample.timestamp().seconds() == timestamp) {
                    return &gpu_sample;
                }
            }

            // Create a new sample if none have been found
            auto new_sample = current_sample_group_->add_gpu_samples();
            new_sample->set_device_name(device_name);
            new_sample->mutable_timestamp()->set_seconds(timestamp);
            return new_sample;
        }
    };
}

#endif //SEM_DEVICE_SAMPLE_AGGREGATOR_HPP
