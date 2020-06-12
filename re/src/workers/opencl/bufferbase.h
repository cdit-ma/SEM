#ifndef RE_OPENCL_BUFFERBASE_H
#define RE_OPENCL_BUFFERBASE_H

#include "genericbuffer.h"

namespace cl {
class Buffer;
}

namespace Re {

namespace OpenCL {

class BufferBase {
    public:
    bool IsValid() const;
    int GetID() const;
    size_t GetByteSize() const;

    void Track(Manager& manager);
    void Untrack(Manager& manager);
    void Release(Manager& manager);

    cl::Buffer& GetBackingRef() const;

    protected:
    BufferBase() = default;
    BufferBase(const Manager& manager,
               size_t num_bytes,
               const std::optional<unsigned long>& flags = (std::nullopt_t)std::nullopt);

    std::unique_ptr<GenericBuffer> buffer_;
    int id_ = -1;

    virtual void LogError(const Worker& worker_reference,
                          std::string function_name,
                          std::string error_message,
                          int cl_error_code) const;
    virtual void LogError(const Worker& worker_reference, std::string function_name, std::string error_message) const;
};

class InvalidBufferException : public BufferException {
    public:
    InvalidBufferException(const std::string& message);
};

class InvalidBufferSizeException : public BufferException {
    public:
    InvalidBufferSizeException(const std::string& message);
};

} // namespace OpenCL

} // namespace Re

#endif // RE_OPENCL_BUFFERBASE_H