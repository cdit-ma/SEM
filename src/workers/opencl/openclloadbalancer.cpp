#include "openclloadbalancer.h"
#include "openclutilities.h"

#include <iostream>
#include <algorithm>

OpenCLLoadBalancer::OpenCLLoadBalancer(const std::vector<unsigned int>& device_ids) {
    for (const auto& device_id : device_ids) {
        device_ids_.emplace(device_id);
    }
}

unsigned int OpenCLLoadBalancer::RequestDevice() {
    std::lock_guard<std::mutex> guard(mutex_);
    /*int cur_min = INT_MAX;
    const cl::Device* best_device = nullptr;
    for (auto device_pair : devices_) {
        if (device_pair.second < cur_min) {
            best_device = &device_pair.first;
            cur_min = device_pair.second;
        }
    }*/

    // Note: More copy operations than theoretically necessary, can be fixed by implementing custom priority_queue 
    /*auto best_device = device_ids_.top();
    device_ids_.pop();
    best_device.num_jobs_++;
    device_ids_.push(best_device);
    return best_device.device_id_;*/

    DeviceJobList djl = *device_ids_.begin();
    device_ids_.erase(device_ids_.begin());
    djl.num_jobs_++;
    device_ids_.insert(djl);
    return djl.device_id_;
}

void OpenCLLoadBalancer::ReleaseDevice(unsigned int dev_id) {
    std::lock_guard<std::mutex> guard(mutex_);

    for (auto& djl : device_ids_) {
        if (djl.device_id_ == dev_id) {
            DeviceJobList updated_entry(djl);
            updated_entry.num_jobs_--;
            device_ids_.erase(djl);
            device_ids_.insert(updated_entry);
            break;
        }
    }
}