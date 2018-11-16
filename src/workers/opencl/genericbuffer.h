#ifndef GENERICBUFFER_H
#define GENERICBUFFER_H

#include <memory>

#include <core/worker.h>

namespace cl {
    class Buffer;
}

class OpenCLManager;
class OpenCLDevice;

class 
GenericBuffer {
public:
    GenericBuffer(const Worker& worker, OpenCLManager& manager, size_t size);

    void Release();

    bool ReadData(const Worker& worker, void* dest, size_t length, const OpenCLDevice& device, bool blocking=true) const;
    bool WriteData(const Worker& worker, const void* source, size_t size, const OpenCLDevice& device, bool blocking=true);
   
    bool is_valid() const;
    size_t GetSize() const;
    const Worker& GetInitialWorker() const;
    
    cl::Buffer& GetBackingRef() const;

protected:
 
    void LogError(const Worker& worker, std::string function_name, std::string error_message, int cl_error_code) const;
    void LogError(const Worker& worker, std::string function_name, std::string error_message) const;

    bool valid_ = false;
    OpenCLManager& manager_;
    std::unique_ptr<cl::Buffer> buffer_;
    size_t size_;

    const Worker& worker_reference_;
};

#endif