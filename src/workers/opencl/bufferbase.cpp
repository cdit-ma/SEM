#include "bufferbase.h"

#include "manager.h"
#include "utilities.h"

using namespace Re::OpenCL;

BufferBase::BufferBase(const Manager& manager, size_t num_bytes, const std::optional<unsigned long>& flags)
{
    buffer_ = std::make_unique<GenericBuffer>(manager, num_bytes, flags);
    id_ = Manager::invalid_buffer_id_;
};

bool BufferBase::IsValid() const
{
    if(buffer_ == nullptr) {
        return false;
    }
    return buffer_->is_valid();
}

int BufferBase::GetID() const { return id_; }

size_t BufferBase::GetByteSize() const
{
    if(!IsValid()) {
        throw InvalidBufferException("Attempting to retrieve size of an invalid Buffer");
    }
    return buffer_->GetSize();
}

void BufferBase::Track(Manager& manager)
{
    id_ = Manager::BufferAttorney::GetNewBufferID(manager, *this);
    if(id_ == Manager::invalid_buffer_id_) {
        throw OpenCLException("Failed to obtain tracking ID for buffer from OpenCLManager", CL_INVALID_MEM_OBJECT);
    }
}
void BufferBase::Untrack(Manager& manager)
{
    if(!IsValid()) {
        throw std::runtime_error("Trying to untrack an invalid OpenCL buffer");
    }
    Manager::BufferAttorney::ReleaseBufferID(manager, *this);
}

void BufferBase::Release(Manager& manager)
{
    if(!IsValid()) {
        throw std::runtime_error("Trying to release an invalid OpenCL buffer");
    }
    buffer_->Release();
}

cl::Buffer& BufferBase::GetBackingRef() const
{
    if(!IsValid()) {
        throw std::runtime_error("Attempting to get backing reference for an invalid Buffer");
    }
    return buffer_->GetBackingRef();
}

void BufferBase::LogError(const Worker& worker_reference,
                          std::string function_name,
                          std::string error_message,
                          int cl_error_code) const
{
    LogOpenCLError(worker_reference, "Buffer::" + function_name, error_message, cl_error_code);
}

void BufferBase::LogError(const Worker& worker_reference, std::string function_name, std::string error_message) const
{
    LogOpenCLError(worker_reference, "Buffer::" + function_name, error_message);
}

InvalidBufferException::InvalidBufferException(const std::string& message) :
    BufferException(message, CL_INVALID_MEM_OBJECT)
{
}

InvalidBufferSizeException::InvalidBufferSizeException(const std::string& message) :
    BufferException(message, CL_INVALID_BUFFER_SIZE)
{
}