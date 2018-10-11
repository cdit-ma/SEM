#ifndef OPENCLBUFFERBASE_H
#define OPENCLBUFFERBASE_H

#include "genericbuffer.h"

namespace cl {
    class Buffer;
}

class OpenCLBufferBase {
public:

    bool IsValid() const;
    int GetID() const;
    size_t GetByteSize() const;

    void Track(const Worker& worker, OpenCLManager& manager);
    void Untrack(OpenCLManager& manager);
    void Release(OpenCLManager& manager);
    
    cl::Buffer& GetBackingRef() const;

protected:
    OpenCLBufferBase() = default;
    OpenCLBufferBase(const Worker& worker, OpenCLManager& manager, size_t num_bytes);

    std::shared_ptr<GenericBuffer> buffer_;
    int id_ = -1;

    virtual void LogError(const Worker& worker_reference,
        std::string function_name,
        std::string error_message,
        int cl_error_code) const;
    virtual void LogError(const Worker& worker_reference,
        std::string function_name,
        std::string error_message) const;
};

#endif