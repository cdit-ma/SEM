#ifndef GENERICBUFFER_H
#define GENERICBUFFER_H

#include <memory>

#include <core/worker.h>
#include <iostream>

namespace cl {
    class Buffer;
}

class OpenCLManager;
class OpenCLDevice;

class GenericBuffer {
public:
    GenericBuffer(OpenCLManager& manager, size_t size, Worker* worker_ref);
    virtual ~GenericBuffer();
    GenericBuffer(const GenericBuffer&) = default;
    GenericBuffer(GenericBuffer&&) = default;
    GenericBuffer& operator=(const GenericBuffer& rhs) & = default;
    GenericBuffer& operator=(GenericBuffer&& rhs) & = default;
    
    bool is_valid() const;
    size_t GetSize() const;
    int GetID() const;
    Worker* GetInitialWorker() const;
    
    const cl::Buffer& GetBackingRef() const;

protected:

    bool ReadData(void* dest, size_t length, const OpenCLDevice& device, bool blocking=true, Worker* worker_reference=NULL) const;
    bool WriteData(const void* source, size_t size, const OpenCLDevice& device, bool blocking=true, Worker* worker_reference=NULL);
    
    virtual void LogError(Worker* worker_reference, std::string function_name, std::string error_message, int cl_error_code) const;
    virtual void LogError(Worker* worker_reference, std::string function_name, std::string error_message) const;

    int id_ = -1;
    bool valid_ = false;
    OpenCLManager& manager_;
    std::shared_ptr<cl::Buffer> buffer_;
    size_t size_;

    Worker* worker_reference_;
};

#endif