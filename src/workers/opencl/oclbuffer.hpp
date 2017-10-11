#ifndef OCLBUFFER_H
#define OCLBUFFER_H

//#include "openclutilities.h"
#include "openclmanager.h"
#include "genericbuffer.h"

#include <vector>

class OpenCLManager;


template <typename T>
class OCLBuffer : public GenericBuffer {
    static_assert(std::is_pod<T>::value,
            "Can't have non-primitive data types buffered in current implementation");
public:
    OCLBuffer(OpenCLManager* manager, size_t num_elements, Worker* worker_reference=0);
    OCLBuffer(OpenCLManager* manager, const std::vector<T>& data,
            Worker* worker_reference=0);
            
    ~OCLBuffer();

    bool WriteData(const std::vector<T>& data, bool blocking=true,
            Worker* worker_reference=NULL);

    const std::vector<T> ReadData(bool blocking=true,
            Worker* worker_reference=NULL);

    size_t GetNumElements() const;

    Worker* GetWorkerReference() const;

private:
    /*template <typename T>
    friend void OpenCLManager::ReleaseBuffer(OCLBuffer<T>* buffer,
            Worker* worker_reference);*/

    static void LogError(Worker* worker_reference,
        std::string function_name,
        std::string error_message,
        int cl_error_code);
    static void LogError(Worker* worker_reference,
        std::string function_name,
        std::string error_message);


    //Worker* worker_reference_ = 0;
    //OpenCLManager* manager_ = nullptr;
};


template <typename T>
OCLBuffer<T>::OCLBuffer(OpenCLManager* manager, size_t num_elements, Worker* worker_reference)
    : GenericBuffer(manager, num_elements*sizeof(T), worker_reference) {

}

template <typename T>
OCLBuffer<T>::OCLBuffer(OpenCLManager* manager, const std::vector<T>& data, Worker* worker_reference)
    : OCLBuffer(manager, data.size(), worker_reference) {
    WriteData(data, true, worker_reference);
}

template <typename T>
OCLBuffer<T>::~OCLBuffer() {
    if (valid_) {
        valid_ = false;
        manager_->UntrackBuffer(id_);
    }
}

template <typename T>
bool OCLBuffer<T>::WriteData(const std::vector<T>& data, bool blocking, Worker* worker_reference) {

static_assert(!std::is_class<T>::value, "Can't have non-primitive data types buffered in current implementation");

    // Warn if size mismatch, abort if overflow would occur
    if (data.size() !=  GetNumElements()) {
        LogError(worker_reference,
            __func__,
            "Attempting to write vector data to a buffer of different length: "
                +std::to_string(data.size())+" when expecting "+std::to_string(size_));
        if (data.size() > GetNumElements()) {
            return false;
        }
    }
    
    bool write_success = GenericBuffer::WriteData(data.data(), size_, blocking, worker_reference);
    return write_success;
}

template <typename T>
const std::vector<T> OCLBuffer<T>::ReadData(bool blocking, Worker* worker_reference) {
    std::vector<T> data(GetNumElements());

    bool read_success = GenericBuffer::ReadData(data.data(), size_, blocking, worker_reference);
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
Worker* OCLBuffer<T>::GetWorkerReference() const{
    return worker_reference_;
}

template <typename T>
void OCLBuffer<T>::LogError(Worker* worker_reference,
    std::string function_name,
    std::string error_message,
    int cl_error_code)
{
    LogOpenCLError(worker_reference,
        "OCLBuffer::" + function_name,
        error_message,
        cl_error_code);
}

template <typename T>
void OCLBuffer<T>::LogError(Worker* worker_reference,
    std::string function_name,
    std::string error_message)
{
    LogOpenCLError(worker_reference,
        "OCLBuffer::" + function_name,
        error_message);
}

#endif // OCLBUFFER_H