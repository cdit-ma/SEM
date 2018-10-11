#ifndef OPENCLBUFFER_H
#define OPENCLBUFFER_H

#include "openclmanager.h"
#include "openclbufferbase.h"
#include "opencldevice.h"

#include <vector>

class OpenCLManager;

template <typename T>
class OpenCLBuffer : public OpenCLBufferBase {
    static_assert(std::is_pod<T>::value,
            "Can't have non-primitive data types buffered in current implementation");
    static_assert(!std::is_same<T, bool>::value, "std::vector<bool> is not guaranteed to conform to STL container requirements");
public:
    OpenCLBuffer() = default;
    OpenCLBuffer(const Worker& worker, OpenCLManager& manager, size_t num_elements);
    OpenCLBuffer(const Worker& worker, OpenCLManager& manager, const std::vector<T>& data, const OpenCLDevice& device, bool blocking=true);

    bool WriteData(const Worker& worker, const std::vector<T>& data, const OpenCLDevice& device, bool blocking=true);

    const std::vector<T> ReadData(const Worker& worker, const OpenCLDevice& device, bool blocking=true) const;

    size_t GetNumElements() const;

};


template <typename T>
OpenCLBuffer<T>::OpenCLBuffer(const Worker& worker, OpenCLManager& manager, size_t num_elements)
    : OpenCLBufferBase(worker, manager, num_elements*sizeof(T)) {
    
    static_assert(!std::is_class<T>::value, "Can't have non-primitive data types buffered in current implementation");
}

template <typename T>
OpenCLBuffer<T>::OpenCLBuffer(const Worker& worker, OpenCLManager& manager, const std::vector<T>& data, const OpenCLDevice& device, bool blocking)
    : OpenCLBuffer(worker, manager, data.size()) {
    bool write_success = WriteData(worker, data, device, blocking);
    if (!write_success) {
        throw std::runtime_error("Unable to write data to buffer during creation");
    }
}

template <typename T>
bool OpenCLBuffer<T>::WriteData(const Worker& worker, const std::vector<T>& data, const OpenCLDevice& device, bool blocking) {

    // Warn if size mismatch, abort if overflow would occur
    if (data.size() != GetNumElements()) {
        LogError(worker,
            __func__,
            "Warning: Attempting to write vector data to a buffer of different length: "
                +std::to_string(data.size())+" when expecting "+std::to_string(GetNumElements()));
        if (data.size() > GetNumElements()) {
            LogError(worker,
                __func__,
                "Skipping buffer write");
            return false;
        }
        LogError(worker,
            __func__,
            "Continuing on despite mismatch (requested size is smaller than available memory space)");
    }
    
    bool write_success = buffer_->WriteData(worker, data.data(), buffer_->GetSize(), device, blocking);
    return write_success;
}

template <typename T>
const std::vector<T> OpenCLBuffer<T>::ReadData(const Worker& worker, const OpenCLDevice& device, bool blocking) const {
    std::vector<T> data(GetNumElements());

    bool read_success = buffer_->ReadData(worker, data.data(), buffer_->GetSize(), device, blocking);
    if (!read_success) {
        data.clear();
    }

    return data;
}

template <typename T>
size_t OpenCLBuffer<T>::GetNumElements() const {
    return buffer_->GetSize()/sizeof(T);
}

#endif // OpenCLBuffer_H