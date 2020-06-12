//
// Created by Jackson Michael on 1/5/20.
//

#ifndef RE_CUFFTHANDLE_CUH
#define RE_CUFFTHANDLE_CUH

#include "fftexecutor.h"

#include <devicelist.cuh>

#include <cufftXt.h>
#include <thrust/host_vector.h>

#include <memory>
#include <vector>

namespace cditma {
namespace cuda {

/**
 * CUFFT currently exposes std::vector in interfaces as it is the most widely supported collection
 * type that offers contiguously allocated memory
 */
template<typename T> using CUFFTVec = std::vector<T>;

/**
 * CUFFT implementation of the FFTExecutor interface, capable of distributing computation across
 * multiple GPUs
 */
class CUFFTHandle : public FFTExecutor<CUFFTVec<float>> {
public:
    /**
     * Create and initialise a CUFFTHandle from a set of devices
     * @param devices The list of CUDA devices to be used for performing FFTs
     */
    explicit CUFFTHandle(DeviceList& devices);
    ~CUFFTHandle();

    /**
     * Perform an FFT on the given data, distrubted across the associated GPUs.
     * @param in_data A continuous array of interleaved complex numbers. For example, a
     * vector containing N complex samples would look like:
     * | r0 | i0 | r1 | i1 | r2 | i2 | ... | rN | iN |
     * @return the result of the FFT, also in interleaved complex number format
     */
    auto FFT(const CUFFTVec<float>& in_data) -> CUFFTVec<float> override;

private:
    std::unique_ptr<cufftHandle> handle_;
    size_t* worksizes = nullptr;
};

} // namespace cuda
} // namespace cditma

#endif