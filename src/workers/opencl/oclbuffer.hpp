#ifndef OCLBUFFER_H
#define OCLBUFFER_H

//#include "openclutilities.h"
#include "openclmanager.h"
#include "genericbuffer.h"

#include <vector>

class OpenCLManager;
class OpenCLDevice;

template <typename T>
class OCLBuffer : public GenericBuffer {
    static_assert(std::is_pod<T>::value,
            "Can't have non-primitive data types buffered in current implementation");
    static_assert(!std::is_same<T, bool>::value, "std::vector<bool> is not guaranteed to conform to STL container requirements");
public:
    OCLBuffer(OpenCLManager& manager, size_t num_elements, Worker* worker_reference=0);
    OCLBuffer(OpenCLManager& manager, const std::vector<T>& data, const OpenCLDevice& device, bool blocking=true,  Worker* worker_reference=0);

    bool WriteData(const std::vector<T>& data, const OpenCLDevice& device, bool blocking=true, Worker* worker_reference=NULL);

    const std::vector<T> ReadData(const OpenCLDevice& device, bool blocking=true, Worker* worker_reference=NULL) const;

    size_t GetNumElements() const;

protected:

    virtual void LogError(Worker* worker_reference,
        std::string function_name,
        std::string error_message,
        int cl_error_code) const;
    virtual void LogError(Worker* worker_reference,
        std::string function_name,
        std::string error_message) const;

};


template <typename T>
OCLBuffer<T>::OCLBuffer(OpenCLManager& manager, size_t num_elements, Worker* worker_reference)
    : GenericBuffer(manager, num_elements*sizeof(T), worker_reference) {

}

template <typename T>
OCLBuffer<T>::OCLBuffer(OpenCLManager& manager, const std::vector<T>& data, const OpenCLDevice& device, bool blocking, Worker* worker_reference)
    : OCLBuffer(manager, data.size(), worker_reference) {
    WriteData(data, device, blocking, worker_reference);
}

template <typename T>
bool OCLBuffer<T>::WriteData(const std::vector<T>& data, const OpenCLDevice& device, bool blocking, Worker* worker_reference) {

static_assert(!std::is_class<T>::value, "Can't have non-primitive data types buffered in current implementation");

    // Warn if size mismatch, abort if overflow would occur
    if (data.size() !=  GetNumElements()) {
        LogError(worker_reference,
            __func__,
            "Attempting to write vector data to a buffer of different length: "
                +std::to_string(data.size())+" when expecting "+std::to_string(GetNumElements()));
        if (data.size() > GetNumElements()) {
            return false;
        }
    }
    
    bool write_success = GenericBuffer::WriteData(data.data(), size_, device, blocking, worker_reference);
    //bool write_success = GenericBuffer::WriteData(&(front_ref), size_, blocking, worker_reference);
    return write_success;
}

template <typename T>
const std::vector<T> OCLBuffer<T>::ReadData(const OpenCLDevice& device, bool blocking, Worker* worker_reference) const {
    std::vector<T> data(GetNumElements());

    bool read_success = GenericBuffer::ReadData(data.data(), size_, device, blocking, worker_reference);
    //bool read_success = GenericBuffer::ReadData(&(front_ref), size_, blocking, worker_reference);
    if (!read_success) {
        data.clear();
    }

    return data;
}

template <typename T>
size_t OCLBuffer<T>::GetNumElements() const {
    return size_/sizeof(T);
}

template <typename T>
void OCLBuffer<T>::LogError(Worker* worker_reference, std::string function_name, std::string error_message, int cl_error_code) const {
    LogOpenCLError(worker_reference,
        "OCLBuffer::" + function_name,
        error_message,
        cl_error_code);
}

template <typename T>
void OCLBuffer<T>::LogError(Worker* worker_reference, std::string function_name, std::string error_message) const {
    LogOpenCLError(worker_reference,
        "OCLBuffer::" + function_name,
        error_message);
}

#endif // OCLBUFFER_H