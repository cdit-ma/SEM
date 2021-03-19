//
// Created by Jackson Michael on 8/9/20.
//

#ifndef SEM_FFT_DATA_HPP
#define SEM_FFT_DATA_HPP

#include <cstring>
#include <vector>

#include "serialized.hpp"

#include "packet_headers.hpp"

#include <iostream>
#include "boost/endian/conversion.hpp"

namespace sem::fft_accel::data {

    template<typename SampleType, size_t NumElements>
    class serialized_fft_data;

    /**
     * Simple implementation of a view into a vector to stand in for standard library support in C++20.
     * Used to provide a handy way of passing around iterators over a particular subsection of a vector. Pay note to
     * the convention that end() will NOT return the last element in the subsection, but instead points to the element
     * after the last included element.
     * Lifetime requirements: the underlying source vector supplied to this class must outlive the range.
     * TODO: Replace with a STL equivalent, particularly if/when C++20 is adopted
     * @tparam SampleType
     */
    template<typename ElementType>
    class vector_range {
    public:
        constexpr vector_range(const std::vector<ElementType> &source_vector, size_t start_index, size_t final_index) :
                source_vec(source_vector), begin_index(start_index), end_index(final_index) {};

        constexpr vector_range(const std::vector<ElementType> &source_vector) :
                vector_range(source_vector, 0, source_vector.size()) {};

        constexpr auto begin() const {
            auto begin_iter = source_vec.begin();
            std::advance(begin_iter, begin_index);
            return begin_iter;
        }

        constexpr auto end() const {
            auto end_iter = source_vec.begin();
            std::advance(end_iter, end_index);
            return end_iter;
        }

    private:
        const std::vector<ElementType> &source_vec;
        size_t begin_index;
        size_t end_index;
    };

    /**
     * This class represents a data packet transmitted as part of the FFT Acceleration Protocol.
     * A single request may be made from one or more of these packets, with each packet's order in the request
     * denoted by the sequence number beginning from 0.
     * @tparam SampleType The type of primitive data elements being used to store FFT sample values
     */
    template<typename SampleType>
    class fft_data_packet {
    public:
        constexpr static size_t max_elements = 256;
        using SerializedPacket = serialized_fft_data<SampleType, max_elements>;

        fft_data_packet(const data::vector_range<SampleType> &data_range, uint8_t request_id,
                        uint8_t sequence_num) :
                fft_data_(data_range.begin(), data_range.end()),
                header_data_(request_id, sequence_num, max_elements){
            if (fft_data_.size() > max_elements) {
                throw std::invalid_argument("Attempting to create fft_data_packet from vector that is too large");
            }
        };

        explicit fft_data_packet(SerializedPacket &serialized_data);

        [[nodiscard]] uint8_t sequence_number() const;

        [[nodiscard]] uint8_t request_id() const;

        [[nodiscard]] const std::vector<SampleType> &payload_data() const;

    private:
        data_packet_header header_data_;

        std::vector<SampleType> fft_data_;
    };

    template<typename SampleType, size_t NumElements = fft_data_packet<SampleType>::max_elements>
    class serialized_fft_data : public Serialized<fft_data_packet<SampleType>> {
    public:
        constexpr static size_t header_byte_length = data_packet_header::serialized_byte_length;
        constexpr static size_t byte_size_from_num_elements(size_t num_elements) {
            return num_elements * sizeof(SampleType) + header_byte_length;
        }

        using native_packet_type = fft_data_packet<SampleType>;
        constexpr static auto packet_byte_size = byte_size_from_num_elements(NumElements);
        using byte_array = std::array<std::byte, packet_byte_size>;

        serialized_fft_data() = default;

        explicit serialized_fft_data(native_packet_type native_packet);

        explicit serialized_fft_data(byte_array data_buffer) : data_(data_buffer) {};

        native_packet_type deserialize() final;

        [[nodiscard]] constexpr byte_span bytes() final;

    private:
        byte_array data_;
    };

    template<typename SampleType, size_t NumElements>
    serialized_fft_data<SampleType, NumElements>::serialized_fft_data(native_packet_type native_packet) {
        data_.at(0) = byte_encoding(packet_type::data);
        data_.at(1) = static_cast<std::byte>(native_packet.sequence_number());
        data_.at(2) = static_cast<std::byte>(native_packet.request_id());
        // 3rd byte currently unused, formerly used for CSR
        // TODO: Thoroughly investigate endianness of remaining fields to be serialized
        uint16_t size_as_big_endian = boost::endian::native_to_big(native_packet.payload_data().size());
        reinterpret_cast<uint16_t&>(data_.at(4)) = size_as_big_endian;

        constexpr auto payload_byte_length = NumElements * sizeof(SampleType);
        memcpy(&(*(data_.begin() + 6)), &(*native_packet.payload_data().begin()), payload_byte_length);

        // Note: the &(*(iterator)) syntax is needed for MSVC compatibility
        auto float_view = reinterpret_cast<std::array<float, NumElements>*>(&(*(data_.begin() + 6)));
        for (auto& sample_value : *float_view) {
            boost::endian::native_to_big_inplace(sample_value);
        }
    }

    template<typename SampleType, size_t NumElements>
    fft_data_packet<SampleType> serialized_fft_data<SampleType, NumElements>::deserialize() {
        return fft_data_packet<SampleType>{*this};
    }

    template<typename SampleType, size_t NumElements>
    constexpr byte_span serialized_fft_data<SampleType, NumElements>::bytes() {
        return byte_span(data_);
    }

};

#endif //SEM_FFT_DATA_HPP
