

#include "genericbuffer.h"
#include "openclutilities.h"
#include "openclmanager.h"


GenericBuffer::GenericBuffer(const Worker& worker, OpenCLManager& manager, size_t size) :
    manager_(manager), size_(size), worker_reference_(worker) {

    if (size ==0) {
        throw std::invalid_argument("Unable to create an OpenCL buffer of length 0");
    }

    cl_int err;

    buffer_ = std::unique_ptr<cl::Buffer>(new cl::Buffer(manager_.GetContext(), CL_MEM_READ_WRITE, size_, NULL, &err));
    if (err != CL_SUCCESS) {
        throw OpenCLException("Unable to create an OpenCL buffer", err);
	}

    valid_ = true;
}

void GenericBuffer::Release() {
    if (!valid_) {
        throw std::runtime_error("Trying to release an invalid openCL buffer");
    }
    valid_ = false;
    buffer_.reset();
}


bool GenericBuffer::is_valid() const{
    return valid_;
}

size_t GenericBuffer::GetSize() const {
    return size_;
}

const Worker& GenericBuffer::GetInitialWorker() const {
    return worker_reference_;
}

cl::Buffer& GenericBuffer::GetBackingRef() const {
    return *(buffer_.get());
}


bool GenericBuffer::ReadData(const Worker& worker, void* dest, size_t size, const OpenCLDevice& device, bool blocking) const {
    cl_int err;

    if (!valid_) {
        LogError(worker,
            __func__,
            "Trying to read from an invalid OpenCL buffer");
        return false;
    }

    if (size == 0) {
        LogError(worker,
            __func__,
            "Trying to read data of length 0 to buffer");
        return false;
    }

    err = device.GetQueue().GetRef().enqueueReadBuffer(GetBackingRef(), blocking, 0, size, dest);
    if(err != CL_SUCCESS){
        LogError(worker,
            __func__,
            "An error occurred while copying data from an OpenCL buffer",
            err);
        return false;
    }

    return true;
}

bool GenericBuffer::WriteData(const Worker& worker, const void* source, size_t size, const OpenCLDevice& device, bool blocking) {
    cl_int err;

    if (!valid_) {
        LogError(worker,
            __func__,
            "Trying to write to an invalid OpenCL buffer");
        return false;
    }

    if (size == 0) {
        LogError(worker,
            __func__,
            "Trying to write length 0 data to buffer");
        return false;
    }

    err = device.GetQueue().GetRef().enqueueWriteBuffer(GetBackingRef(), blocking, 0, size, source);
    if(err != CL_SUCCESS){
        LogError(worker,
            __func__,
            "An error occurred while copying data to an OpenCL buffer",
            err);
        return false;
    }

    return true;
}

void GenericBuffer::LogError(const Worker& worker, std::string function_name, std::string error_message,
    int cl_error_code) const
{
    LogOpenCLError(worker,
        "GenericBuffer::" + function_name,
        error_message,
        cl_error_code);
}

void GenericBuffer::LogError(const Worker& worker, std::string function_name, std::string error_message) const
{
    LogOpenCLError(worker,
        "GenericBuffer::" + function_name,
        error_message);
}