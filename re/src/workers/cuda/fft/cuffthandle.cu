//
// Created by Jackson Michael on 1/5/20.
//

#include "cuffthandle.cuh"

#include <iostream>

#include <cufftXt.h>
#include <thrust/device_vector.h>

#include "cufft_error.h"

namespace cditma {
namespace cuda {

using cditma::cuda::cufft::throw_on_error;

CUFFTHandle::CUFFTHandle(DeviceList& devices) : handle_(std::make_unique<cufftHandle>())
{
    // Perform the basic initialisation of the fft execution plan
    throw_on_error(cufftCreate(handle_.get()));

    // Point the newly created plan at the list of provided devices
    throw_on_error(cufftXtSetGPUs(*handle_, devices.Size(), devices.GetMutableDevices()));

    // Create the worksizes for each GPU
    worksizes = new size_t[devices.Size()];
}

CUFFTHandle::~CUFFTHandle()
{
    delete[] worksizes;
    cufftDestroy(*handle_);
}

CUFFTVec<float> CUFFTHandle::FFT(const CUFFTVec<float>& in_data)
{
    size_t num_samples = in_data.size() / 2;

    // Make the plan now that we know the size of the input data (worksizes calculated earlier based
    // on number of GPUs
    if(worksizes == nullptr) {
        throw std::runtime_error("null worksize encountered when running FFT, was CUFFTHandle "
                                 "configured?");
    }
    throw_on_error(cufftMakePlan1d(*handle_, num_samples, CUFFT_C2C, 1, worksizes));

    // Copy the input data to each device
    // Note: I'm not sure of the exact reason, however CUFFT_XT_FORMAT_INPLACE should be used
    //  instead of CUFFT_XT_FORMAT_INPUT; doing otherwise appears to return CUFFT_INVALID_TYPE when
    //  the following copy occurs, with the description "No longer used". The use of INPUT might be
    //  deprecated but not documented?
    cudaLibXtDesc* device_data_in;
    throw_on_error(
        cufftXtMalloc(*handle_, (cudaLibXtDesc**)&device_data_in, CUFFT_XT_FORMAT_INPLACE));
    throw_on_error(
        cufftXtMemcpy(*handle_, device_data_in, (void*)in_data.data(), CUFFT_COPY_HOST_TO_DEVICE));

    // Allocate space for the result on each device
    cudaLibXtDesc* device_data_out;
    throw_on_error(cufftXtMalloc(*handle_, &device_data_out, CUFFT_XT_FORMAT_OUTPUT));

    // Execute the FFT
    throw_on_error(
        cufftXtExecDescriptorC2C(*handle_, device_data_in, device_data_out, CUFFT_FORWARD));

    // Create vector on host CPU using the same length as the input vector
    std::vector<float> out_data(in_data.size());

    // Copy back from device
    throw_on_error(
        cufftXtMemcpy(*handle_, out_data.data(), device_data_out, CUFFT_COPY_DEVICE_TO_HOST));

    // Clean up the memory we have allocated
    // TODO: Wrap these allocations in an exception safe class to avoid leaking memory if we throw
    throw_on_error(cufftXtFree(device_data_out));
    throw_on_error(cufftXtFree(device_data_in));

    return out_data;
}

} // namespace cuda
} // namespace cditma