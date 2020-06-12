//
// Created by Jackson Michael on 29/4/20.
//

#include "cudaworker.h"

#include "fft/cuffthandle.cuh"

#include <devicelist.cuh>

#include <core/logger.h>

#include <cuda_runtime.h>

namespace cditma::workers {

CUDAWorker::CUDAWorker(const BehaviourContainer& container, const std::string& inst_name) :
    Worker(container, GET_FUNC, inst_name), cufft_handle(nullptr)
{
}

std::vector<float> CUDAWorker::FFT(const std::vector<float>& in_data, bool& success_flag)
{
    // Assume failure until success
    success_flag = false;

    // Obtain a id for this particular FFT call and mark that it has begun
    int work_id = get_new_work_id();
    Log(GET_FUNC, Logger::WorkloadEvent::STARTED, work_id);

    // Make sure we have an FFT handle to actually perform the computation
    if(cufft_handle == nullptr) {
        Log(GET_FUNC, Logger::WorkloadEvent::ERROR, work_id,
            "CUFFT was not initialised - was the CUDAWorker successfully configured?");
        success_flag = false;
        return std::vector<float>();
    }

    try {
        std::vector<float>&& out_data = cufft_handle->FFT(in_data);
        Log(GET_FUNC, Logger::WorkloadEvent::FINISHED, work_id);
        success_flag = true;
        return out_data;
    } catch(const std::exception& e) {
        Log(GET_FUNC, Logger::WorkloadEvent::ERROR, work_id,
            "An exception was thrown while exectuing FFT:\n"+std::string(e.what()));
        success_flag = false;
        return std::vector<float>();
    }
}

void CUDAWorker::HandleConfigure()
{
    if(cufft_handle != nullptr) {
        throw std::runtime_error("CUDAWorker already appears to be configured (FFT plan is not "
                                 "null); should not be configured again without being terminated "
                                 "first");
    }
    cuda::DeviceList device_list;
    cufft_handle = new cditma::cuda::CUFFTHandle(device_list);
    Worker::HandleConfigure();
}

const std::string& CUDAWorker::get_version() const
{
    const static std::string worker_version{"0.9.0"};
    return worker_version;
}

void CUDAWorker::HandleTerminate()
{
    if(cufft_handle == nullptr) {
        throw std::runtime_error("CUDAWorker already appears to be configured (FFT plan is not "
                                 "null); should not be configured again without being terminated "
                                 "first");
    }
    delete cufft_handle;
    Worker::HandleTerminate();
}

} // namespace cditma::workers
