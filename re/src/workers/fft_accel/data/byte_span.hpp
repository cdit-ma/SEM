//
// Created by Jackson Michael on 13/11/20.
//

#ifndef SEM_BYTE_SPAN_H
#define SEM_BYTE_SPAN_H

#include <cstddef>
#include <array>

namespace sem::fft_accel::data {

/**
 * Provides a view into a contiguous array of bytes. Remains valid only so long as the underlying array remains valid.
 * Intended as a simple and minimal replacement for span (C++20 feature) without adding another dependency.
 * TODO: Candidate for replacement if/when we move to C++20 support
 */
    class byte_span {
    public:
        template<size_t Length>
        constexpr
        byte_span(std::array<std::byte, Length>& byte_array) : begin_(&(*byte_array.begin())), length_(Length) {};

        constexpr byte_span(std::byte *begin, size_t length) : begin_(begin), length_(length) {};

        [[nodiscard]] constexpr std::byte *begin() const { return begin_; };

        [[nodiscard]] constexpr std::byte *end() const { return begin_ + length_; };

        [[nodiscard]] constexpr std::byte operator[] (size_t byte_offset) const { return *(begin_ + byte_offset); };
        [[nodiscard]] constexpr std::byte& operator[] (size_t byte_offset) { return *(begin_ + byte_offset); };

        [[nodiscard]] constexpr size_t size() const {return length_;};

        [[nodiscard]] constexpr byte_span subspan(size_t byte_offset) {
            if (byte_offset >= length_) {
                throw std::logic_error(
                        "Attempting to create subspan that would start beyond the end of a byte_span");
            }
            return byte_span(begin_ + byte_offset, length_ - byte_offset);
        };

        [[nodiscard]] constexpr byte_span subspan(size_t byte_offset, size_t length) {
            if (byte_offset >= length_) {
                throw std::out_of_range(
                        "Attempting to create subspan that would start beyond the end of a byte_span");
            }
            if (length > length_) {
                throw std::out_of_range(
                        "Attempting to create subspan that would be longer than the originating byte_span");
            }
            if (byte_offset + length >= length_) {
                throw std::out_of_range("Attempting to create subspan that would over-run the end of a byte_span");
            }
            return byte_span(begin_ + byte_offset, length);
        };

        /**
         * Read from the specified byte offset from the beginning of the byte array, interpreting it as the requested
         * type.
         * NOTE: No alignment or endianness checking is done for types larger than a single byte, such checking is the
         * responsibility of the caller.
         * @tparam T The type that the byte sequence should be interpreted as
         * @param byte_offset The number of bytes that the
         * @return
         */
        template<typename T>
        [[nodiscard]] constexpr T at(size_t byte_offset) const {
            if (byte_offset + sizeof(T) > length_) {
                throw std::out_of_range("Attempting to read beyond the end of a byte_span");
            }
            return *reinterpret_cast<T *>((begin_ + byte_offset));
        }

    private:
        std::byte *begin_;
        size_t length_;
    };

};

#endif //SEM_BYTE_SPAN_H
