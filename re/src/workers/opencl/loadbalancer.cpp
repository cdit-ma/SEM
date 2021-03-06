#include "loadbalancer.h"
#include "utilities.h"

#include <algorithm>
#include <iostream>

using namespace Re::OpenCL;

LoadBalancer::LoadBalancer(const std::vector<unsigned int>& device_ids)
{
    for(const auto& device_id : device_ids) {
        device_ids_.emplace(device_id);
    }
}

unsigned int LoadBalancer::RequestDevice()
{
    std::lock_guard<std::mutex> guard(mutex_);

    DeviceJobList djl = *device_ids_.begin();
    device_ids_.erase(device_ids_.begin());
    djl.num_jobs_++;
    device_ids_.insert(djl);
    return djl.device_id_;
}

void LoadBalancer::ReleaseDevice(unsigned int dev_id)
{
    std::lock_guard<std::mutex> guard(mutex_);

    for(auto& djl : device_ids_) {
        if(djl.device_id_ == dev_id) {
            DeviceJobList updated_entry(djl);
            updated_entry.num_jobs_--;
            device_ids_.erase(djl);
            device_ids_.insert(updated_entry);
            break;
        }
    }
}