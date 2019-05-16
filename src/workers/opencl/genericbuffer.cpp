

#include "genericbuffer.h"
#include "manager.h"
#include "utilities.h"

using namespace Re::OpenCL;

GenericBuffer::GenericBuffer(const Manager& manager, size_t size, const std::optional<unsigned long>& flags) :
    manager_(manager),
    size_(size)
{
    if(size == 0) {
        throw InvalidBufferSizeException("Unable to create an OpenCL buffer of length 0");
    }

    cl_mem_flags buffer_flags;
    if(flags) {
        buffer_flags = flags.value();
    } else {
    }

    cl_int err;

    // buffer_ = std::unique_ptr<cl::Buffer>(new cl::Buffer(manager_.GetContext(), CL_MEM_READ_WRITE, size_, NULL,
    // &err));
    buffer_ = std::unique_ptr<cl::Buffer>(
        new cl::Buffer(manager_.GetContext(), (cl_mem_flags)flags.value_or(CL_MEM_READ_WRITE), size_, NULL, &err));
    if(err != CL_SUCCESS) {
        throw OpenCLException("Unable to create an OpenCL buffer", err);
    }

    valid_ = true;
}

void GenericBuffer::Release()
{
    if(!valid_) {
        throw std::runtime_error("Trying to release an invalid openCL buffer");
    }
    valid_ = false;
    buffer_.reset();
}

bool GenericBuffer::is_valid() const { return valid_; }

size_t GenericBuffer::GetSize() const { return size_; }

cl::Buffer& GenericBuffer::GetBackingRef() const { return *(buffer_.get()); }

void GenericBuffer::ReadData(void* dest, size_t size, const Device& device, bool blocking) const
{
    if(!valid_) {
        throw BufferException("Trying to read from an invalid OpenCL buffer", CL_INVALID_MEM_OBJECT);
    }

    if(size == 0) {
        throw BufferException("Trying to read buffer of length 0", CL_INVALID_BUFFER_SIZE);
    }

    cl_int status = device.GetQueue().GetRef().enqueueReadBuffer(GetBackingRef(), blocking, 0, size, dest);
    if(status != CL_SUCCESS) {
        throw BufferException("An error occurred while copying data from an OpenCL buffer", status);
    }
}

void GenericBuffer::WriteData(const void* source, size_t size, const Device& device, bool blocking)
{
    if(!valid_) {
        throw BufferException("Trying to write to an invalid OpenCL buffer", CL_INVALID_MEM_OBJECT);
    }

    if(size == 0) {
        throw BufferException("Trying to write length 0 data to buffer", CL_INVALID_BUFFER_SIZE);
    }

    cl_int status = device.GetQueue().GetRef().enqueueWriteBuffer(GetBackingRef(), blocking, 0, size, source);
    if(status != CL_SUCCESS) {
        throw BufferException("An error occurred while copying data to an OpenCL buffer", status);
    }
}

void GenericBuffer::LogError(const Worker& worker,
                             std::string function_name,
                             std::string error_message,
                             int cl_error_code) const
{
    LogOpenCLError(worker, "GenericBuffer::" + function_name, error_message, cl_error_code);
}

void GenericBuffer::LogError(const Worker& worker, std::string function_name, std::string error_message) const
{
    LogOpenCLError(worker, "GenericBuffer::" + function_name, error_message);
}