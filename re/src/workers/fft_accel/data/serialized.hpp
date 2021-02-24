//
// Created by Jackson Michael on 11/11/20.
//

#ifndef SEM_SERIALIZATION_HPP
#define SEM_SERIALIZATION_HPP

#include "byte_span.hpp"

namespace sem::fft_accel::data {

/**
 * An interface to be used for classes that represent a bit-stream serialization of some other class/type.
 * Notably, implementations of this interface must provide the ability to deserialize and produce the
 * original type.
 * @tparam T
 */
    template<typename T>
    class Serialized {
    public:
        virtual T deserialize() = 0;

        [[nodiscard]]
        virtual byte_span bytes() = 0;
    };


}

#endif //SEM_SERIALIZATION_HPP
