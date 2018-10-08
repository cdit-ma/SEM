#include "openclbufferbase.h"

#include "openclutilities.h"
#include "openclmanager.h"

OpenCLBufferBase::OpenCLBufferBase(const Worker& worker, OpenCLManager& manager, size_t num_bytes) {

    buffer_ = std::make_shared<GenericBuffer>(worker, manager, num_bytes);
    id_ = OpenCLManager::invalid_buffer_id_;
};


bool OpenCLBufferBase::IsValid() const {
    if (buffer_ == nullptr) {
        return false;
    }
    return buffer_->is_valid();
}

int OpenCLBufferBase::GetID() const {
    return id_;
}

size_t OpenCLBufferBase::GetByteSize() const {
    if (!IsValid()) {
        throw std::runtime_error("Attempting to retrieve size of an invalid OpenCLBuffer");
    }
    return buffer_->GetSize();
}

void OpenCLBufferBase::Track(const Worker& worker, OpenCLManager& manager) {
    id_ = OpenCLManager::BufferAttorney::GetNewBufferID(worker, manager, *this);
    if (id_ == OpenCLManager::invalid_buffer_id_) {
        LogError(worker,
			std::string(__func__),
			"Failed to obtain tracking ID for buffer from OpenCLManager");
        return;
    }
}

void OpenCLBufferBase::Release(OpenCLManager& manager) {
    if (!IsValid()) {
        throw std::runtime_error("Trying to release an invalid OpenCL buffer");
    }
    buffer_->Release();
    OpenCLManager::BufferAttorney::ReleaseBufferID(manager, *this);
}

cl::Buffer& OpenCLBufferBase::GetBackingRef() const {
    if (!IsValid()) {
        throw std::runtime_error("Attempting to get backing reference for an invalid OpenCLBuffer");
    }
    buffer_->GetBackingRef();
}

void OpenCLBufferBase::LogError(const Worker& worker_reference, std::string function_name, std::string error_message, int cl_error_code) const {
    LogOpenCLError(worker_reference,
        "OCLBuffer::" + function_name,
        error_message,
        cl_error_code);
}

void OpenCLBufferBase::LogError(const Worker& worker_reference, std::string function_name, std::string error_message) const {
    LogOpenCLError(worker_reference,
        "OCLBuffer::" + function_name,
        error_message);
}