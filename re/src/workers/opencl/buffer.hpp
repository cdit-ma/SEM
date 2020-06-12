#ifndef RE_OPENCL_BUFFER_H
#define RE_OPENCL_BUFFER_H

#include "bufferbase.h"
#include "device.h"

#include <vector>

#include <iostream>

namespace Re::OpenCL {

class Manager;

template<typename T> class Buffer : public BufferBase {
    static_assert(std::is_pod<T>::value,
                  "Can't have non-primitive data types buffered in current implementation");
    static_assert(!std::is_same<T, bool>::value,
                  "std::vector<bool> is not guaranteed to conform to STL container requirements");

public:
    Buffer() = default;
    Buffer(const Manager& manager,
           size_t num_elements,
           const std::optional<unsigned long>& flags = (std::nullopt_t)std::nullopt);
    Buffer(const Manager& manager,
           const std::vector<T>& data,
           const Device& device,
           bool blocking = true,
           const std::optional<unsigned long>& flags = (std::nullopt_t)std::nullopt);

    bool WriteData(const std::vector<T>& data, const Device& device, bool blocking = true);

    const std::vector<T> ReadData(const Device& device, bool blocking = true) const;

    size_t GetNumElements() const;
};

template<typename T>
Buffer<T>::Buffer(const Manager& manager,
                  size_t num_elements,
                  const std::optional<unsigned long>& flags) :
    BufferBase(manager, num_elements * sizeof(T), flags)
{
    static_assert(!std::is_class<T>::value, "Can't have non-primitive data types buffered in "
                                            "current implementation");
}

template<typename T>
Buffer<T>::Buffer(const Manager& manager,
                  const std::vector<T>& data,
                  const Device& device,
                  bool blocking,
                  const std::optional<unsigned long>& flags) :
    Buffer(manager, data.size(), flags)
{
    try {
        bool write_success = WriteData(data, device, blocking);
    } catch(const OpenCLException& ocle) {
        throw OpenCLException(std::string("Unable to write data to buffer during creation:\n")
                                  + ocle.what(),
                              ocle.ErrorCode());
    }
}

template<typename T>
bool Buffer<T>::WriteData(const std::vector<T>& data, const Device& device, bool blocking)
{
    // Warn if size mismatch, abort if overflow would occur
    if(data.size() != GetNumElements()) {
        /*LogError(worker, __func__,
                 "Warning: Attempting to write vector data to a buffer of different length: "
                     + std::to_string(data.size()) + " when expecting " +
           std::to_string(GetNumElements()));*/
        if(data.size() > GetNumElements()) {
            InvalidBufferSizeException(
                std::string("Attempting to write ") + std::to_string(data.size())
                + std::string(" elements to a buffer containing ")
                + std::to_string(GetNumElements()) + std::string(" elements"));
        }
        /*LogError(worker, __func__,
                 "Continuing on despite mismatch (requested size is smaller than available memory
           space)");*/
    }
    try {
        buffer_->WriteData(data.data(), buffer_->GetSize(), device, blocking);
    } catch(const BufferException& be) {
        throw;
    }
    return true;
}

template<typename T>
const std::vector<T> Buffer<T>::ReadData(const Device& device, bool blocking) const
{
    std::vector<T> data(GetNumElements());

    try {
        buffer_->ReadData(data.data(), buffer_->GetSize(), device, blocking);
    } catch(const BufferException& be) {
        throw BufferException(std::string("BufferException occurred while reading data:\n")
                                  + be.what(),
                              be.ErrorCode());
    }

    return data;
}

template<typename T> size_t Buffer<T>::GetNumElements() const
{
    if(!IsValid()) {
        throw InvalidBufferException("Attempting to get number of elements from an invalid buffer");
    }
    return buffer_->GetSize() / sizeof(T);
}

} // namespace Re::OpenCL

#endif // RE_OPENCL_BUFFER_H