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
    int num_gpus;
    cudaError_t status = cudaGetDeviceCount(&num_gpus);
    if(status != cudaSuccess) {
        throw std::runtime_error("Failed to ger the number of CUDA devices, with code "
                                 + std::to_string(status));
    }
    cuda_devices_.resize(num_gpus);
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