#ifndef OCLBUFFER_H
#define OCLBUFFER_H

#include "openclutilities.h"
#include "openclmanager.h"
#include "genericbuffer.h"

#include <vector>

class OpenCLManager;


template <typename T>
class OCLBuffer : public GenericBuffer {
    static_assert(std::is_pod<T>::value,
            "Can't have non-primitive data types buffered in current implementation");
public:
    OCLBuffer(OpenCLManager* manager, size_t bytes, Worker* worker_reference=0);
    OCLBuffer(OpenCLManager* manager, const std::vector<T>& data,
            Worker* worker_reference=0);
            
    ~OCLBuffer();

    void WriteData(const std::vector<T>& data, bool blocking=false,
            Worker* worker_reference=NULL);

    const std::vector<T> ReadData(bool blocking=false,
            Worker* worker_reference=NULL);

    Worker* GetWorkerReference() const;

    //const cl::Buffer& get_backing_ref() const;

    static void ReleaseAll();

private:
    /*template <typename T>
    friend void OpenCLManager::ReleaseBuffer(OCLBuffer<T>* buffer,
            Worker* worker_reference);*/

    static void LogError(Worker* worker_reference,
        std::string function_name,
        std::string error_message,
        cl_int cl_error_code);
    static void LogError(Worker* worker_reference,
        std::string function_name,
        std::string error_message);


    Worker* worker_reference_ = 0;
    //OpenCLManager* manager_ = nullptr;
};


template <typename T>
OCLBuffer<T>::OCLBuffer(OpenCLManager* manager, size_t num_elements, Worker* worker_reference)
    : GenericBuffer(manager, num_elements), worker_reference_(worker_reference){
    
    cl_int err;

    buffer_ = cl::Buffer(manager_->GetContext(), CL_MEM_READ_WRITE, size_*sizeof(T), NULL, &err);
    if (err != CL_SUCCESS) {
		LogError(worker_reference_,
			std::string(__func__),
			"Failed to create a buffer",
            err);
        valid_ = false;
		return;
	}

    valid_ = true;
}

template <typename T>
OCLBuffer<T>::OCLBuffer(OpenCLManager* manager, const std::vector<T>& data, Worker* worker_reference)
    : OCLBuffer(manager, data.size(), worker_reference) {
    WriteData(data, false, worker_reference);
}

template <typename T>
OCLBuffer<T>::~OCLBuffer() {
    if (valid_) {
        valid_ = false;
        manager_->UntrackBuffer(id_);
    }
}

template <typename T>
void OCLBuffer<T>::WriteData(const std::vector<T>& data, bool blocking, Worker* worker_reference) {

static_assert(!std::is_class<T>::value, "Can't have non-primitive data types buffered in current implementation");

    cl_int err;
    
    // Warn if size mismatch, abort if overflow would occur
    if (data.size() !=  size_) {
        LogError(worker_reference,
            __func__,
            "Attempting to write vector data to a buffer of different length: "
                +std::to_string(data.size())+" when expecting "+std::to_string(size_));
        if (data.size() > size_) {
            return;
        }
    }
    // TODO: allow for non-blocking variant
    err = cl::copy(manager_->GetQueues().at(0), data.begin(), data.end(), buffer_);
    if(err != CL_SUCCESS){
        LogError(worker_reference,
            __func__,
            "An error occurred while copying data to an OpenCL buffer",
            err);
    }
}

template <typename T>
const std::vector<T> OCLBuffer<T>::ReadData(bool blocking, Worker* worker_reference) {

    cl_int err;

    std::vector<T> data(size_);

    // NEEDS TO HAVE LENGTH INITIALISED FIRST
    //err = cl::copy(manager_->GetQueues().at(0), buffer_, data.begin(), data.end());
    err = manager_->GetQueues().at(0).enqueueReadBuffer(buffer_, true, 0, size_*sizeof(T), data.data());
    if(err != CL_SUCCESS){
        LogError(worker_reference,
            __func__,
            "An error occurred while copying data from an OpenCL buffer",
            err);
    }

    return data;
}

template <typename T>
Worker* OCLBuffer<T>::GetWorkerReference() const{
    return worker_reference_;
}

/*template <typename T>
const cl::Buffer& OCLBuffer<T>::get_backing_ref() const {
    return buffer_;
}*/

template <typename T>
void OCLBuffer<T>::LogError(Worker* worker_reference,
    std::string function_name,
    std::string error_message,
    cl_int cl_error_code)
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