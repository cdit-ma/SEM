#ifndef RE_OPENCL_FFTPROVIDER_H
#define RE_OPENCL_FFTPROVIDER_H

#include <vector>

#include "buffer.hpp"
#include "device.h"

#ifndef HAS_DESTRUCTOR_H
#define HAS_DESTRUCTOR_H

#include <type_traits>

/*! The template `has_destructor<T>` exports a
    boolean constant `value that is true iff `T` has
    a public destructor.

    N.B. A compile error will occur if T has non-public destructor.
*/
template<typename T> struct has_destructor {
    /* Has destructor :) */
    template<typename A> static std::true_type test(decltype(std::declval<A>().~A())*) { return std::true_type(); }

    /* Has no destructor :( */
    template<typename A> static std::false_type test(...) { return std::false_type(); }

    /* This will be either `std::true_type` or `std::false_type` */
    typedef decltype(test<T>(0)) type;

    static const bool value = type::value; /* Which is it? */
};

#endif // HAS_DESTRUCTOR_H

namespace Re {

namespace OpenCL {

/**
 * Abstract interface for classes looking to provide OpenCL FFT implementations for various platforms
 **/
class FFTProvider {
    public:
    virtual ~FFTProvider(){};

    /**
     * Perform an in-place FFT calculation using the supplied vector
     **/
    virtual std::vector<float> FFT(std::vector<float>& data, Device& device) = 0;

    /**
     * Perform an in-place FFT calculation using the supplied Buffer
     **/
    virtual Buffer<float> FFT(Buffer<float>& data, Device& device) = 0;

    /**
     * Allows For the graceful destruction of initialised resources such that exceptions may be handled;
     * this function will be called by the destructor of the class with exceptions swallowed to prevent
     * leaking exceptions out of destructors
     **/
    virtual void Teardown() = 0;
};

// template<typename T, typename... Args>
// typename std::enable_if<!has_destructor<T>::value, void>::type* make_provider(Args&&... args)
// {
//     throw std::runtime_error("No template specialisation provided for make_provider");
//     // return new T(std::forward<Args>(args)...);
// }

// template<typename T, typename... Args> typename T* make_provider(Args&&... args)
// {
//     throw std::runtime_error("No template specialisation provided for make_provider");
//     // return new T(std::forward<Args>(args)...);
// }

template<typename T> T* make_provider(const Manager& manager)
{
    throw std::runtime_error("No template specialisation provided for make_provider");
    // return new T(std::forward<Args>(args)...);
}

/*
template<typename T, typename... Args>
typename std::enable_if<has_destructor<T>::value, void>::type* make_provider(Args&&... args)
{
    // throw std::runtime_error("No template specialisation provided for make_provider");
    return new T(std::forward<Args>(args)...);
}
*/

} // namespace OpenCL

} // namespace Re

#endif // RE_OPENCL_FFT_PROVIDER