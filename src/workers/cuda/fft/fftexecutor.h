//
// Created by Jackson Michael on 1/5/20.
//

#ifndef RE_FFTEXECUTOR_H
#define RE_FFTEXECUTOR_H

namespace cditma {
namespace cuda {

/**
 * Interface for classes that execute a Fast Fourier Transform, taking a collection of values and
 * returning the result in an array of the same type.
 * The precise nature of these value types is to be defined by any class implementing this function;
 * ie does ArrayType contain complex values, interleaved real and imaginary values, or just real
 * values?
 * This interface is created with the intent that the elements of the provided array are
 * contiguously allocated, however this is not guaranteed.
 * @tparam ArrayType
 */
template<typename ArrayType> class FFTExecutor {
    /**
     * Perform a Fast Fourier Transform using the supplied data
     * @param in_data A collection of values upon which to perform the transform
     * @return The result of the transform, with the same length as the input.
     */
    virtual ArrayType FFT(const ArrayType& in_data) = 0;
};

} // namespace cuda
} // namespace cditma

#endif // RE_FFTEXECUTOR_H
