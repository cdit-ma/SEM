//
// Created by Jackson Michael on 13/11/20.
//

#ifndef SEM_BYTE_SPAN_H
#define SEM_BYTE_SPAN_H

#include <cstddef>
#include <stdexcept>
#include <array>

namespace sem::fft_accel::data {

/**
 * Provides a view into a contiguous array of bytes. Remains valid only so long as the underlying array remains valid.
 * Intended as a simple and minimal replacement for span (C++20 feature) without adding another dependency.
 * TODO: Candidate for replacement if/when we move to C++20 support
 */
    class byte_span {
    public:
        using iterator = std::array<std::byte,0>::iterator;

        template<size_t Length>
        constexpr explicit byte_span(std::array<std::byte, Length> &byte_array) :
                begin_(byte_array.begin()), end_(byte_array.end()) {};

        constexpr byte_span(iterator begin, iterator end) :
                begin_(begin),
                end_(end) {};


        [[nodiscard]] constexpr iterator begin() const { return begin_; };

        [[nodiscard]] constexpr iterator end() const { return end_; };

        [[nodiscard]] constexpr std::byte operator[](size_t byte_offset) const { return *(begin_ + byte_offset); };

        [[nodiscard]] constexpr std::byte &operator[](size_t byte_offset) { return *(begin_ + byte_offset); };

        [[nodiscard]] constexpr size_t size() const { return std::distance(begin_, end_); };

        [[nodiscard]] constexpr byte_span subspan(size_t byte_offset) {
            if (begin_ + byte_offset >= end_) {
                throw std::out_of_range(
                        "Attempting to create subspan that would start beyond the end of a byte_span");
            }
            return byte_span(begin_ + byte_offset, end_);
        };

        [[nodiscard]] constexpr byte_span subspan(size_t byte_offset, size_t length) {
            auto && new_begin = begin_ + byte_offset;
            if (new_begin >= end_) {
                throw std::out_of_range(
                        "Attempting to create subspan that would start beyond the end of a byte_span");
            }
            if (length > std::distance(begin_, end_)) {
                throw std::out_of_range(
                        "Attempting to create subspan that would be longer than the originating byte_span");
            }
            auto && new_end = new_begin + length;
            if (new_end >= end_) {
                throw std::out_of_range("Attempting to create subspan that would over-run the end of a byte_span");
            }
            return byte_span(new_begin, new_end);
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
            if (byte_offset + sizeof(T) > std::distance(begin_, end_)) {
                throw std::out_of_range("Attempting to read beyond the end of a byte_span");
            }
            return *reinterpret_cast<T *>(begin_ + byte_offset);
        }

    private:
        iterator begin_;
        iterator end_;
    };

};

#endif //SEM_BYTE_SPAN_H
