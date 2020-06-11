//
// Created by Jackson Michael on 3/5/20.
//

#include "devicelist.cuh"

#include <stdexcept>

#include <cuda_runtime.h>

namespace cditma {
namespace cuda {

DeviceList::DeviceList()
{
    // Get the number of GPU devices
    int num_gpus;
    cudaError_t status = cudaGetDeviceCount(&num_gpus);
    if(status != cudaSuccess) {
        throw std::runtime_error("Failed to ger the number of CUDA devices, with code "
                                 + std::to_string(status));
    }

    // Create the vector of devices, with each element equal to the device id (ie the index)
    cuda_devices_.resize(num_gpus);
    for (size_t id=0; id < cuda_devices_.size(); id++) {
        cuda_devices_[id] = id;
    }
}

const int* DeviceList::GetDevices() const
{
    // Return the list of available cuda devices
    return cuda_devices_.data();
}

size_t DeviceList::Size() const
{
    return cuda_devices_.size();
}
int* DeviceList::GetMutableDevices()
{
    return &(*cuda_devices_.begin());
}

} // namespace cuda
} // namespace cditma