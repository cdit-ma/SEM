//
// Created by Jackson Michael on 29/4/20.
//

#ifndef RE_CUDAWORKER_H
#define RE_CUDAWORKER_H

#include <memory>

#include <core/worker.h>

namespace cditma {

namespace cuda {
class CUFFTHandle;
}

namespace workers {

/**
 * CUDAWorker exists to provide a mechanism for executing workloads on NVIDIA GPUs
 */
class CUDAWorker : public Worker {
public:
    /**
     * Constructs the worker in an uninitialised state; call Configure() once all of the required
     * attributes have been set in order to initialise it (see also: HandleConfigure())
     * @param container The BehaviourContainer that this worker will be added to.
     * @param inst_name The display name of this worker
     */
    CUDAWorker(const BehaviourContainer& container, const std::string& inst_name);

    /**
     * Perform an FFT across the input data, distributing the computation across multiple GPUs.
     * @param in_data A vector of floats in complex interleaved format. For example, a vector
     * containing N complex samples would look like:
     * | r0 | i0 | r1 | i1 | r2 | i2 | ... | rN | iN |
     * @param success_flag A boolean flag that can be used to determine whether or not the FFT
     * operation was completed without errors
     * @return The output result as a vector of floats in complex interleaved format
     */
    std::vector<float> FFT(const std::vector<float>& in_data, bool& success_flag);

    const std::string& get_version() const override;

protected:
    void HandleConfigure() override;
    void HandleTerminate() override;

private:
    cditma::cuda::CUFFTHandle* cufft_handle{};
};

} // namespace workers
} // namespace cditma

// Expose CUDAWorker to MEDEA without namespacing
using CUDAWorker = cditma::workers::CUDAWorker;

#endif // RE_CUDAWORKER_H
