#ifndef RE_OPENCL_GENERICBUFFER_H
#define RE_OPENCL_GENERICBUFFER_H

#include "openclexception.h"

#include <memory>

#include <core/worker.h>

namespace cl {
class Buffer;
}

namespace Re {

namespace OpenCL {

class Manager;
class Device;

class GenericBuffer {
    public:
    GenericBuffer(const Manager& manager,
                  size_t size,
                  const std::optional<unsigned long>& flags = (std::nullopt_t)std::nullopt);

    void Release();

    void ReadData(void* dest, size_t length, const Device& device, bool blocking = true) const;
    void WriteData(const void* source, size_t size, const Device& device, bool blocking = true);

    bool is_valid() const;
    size_t GetSize() const;

    cl::Buffer& GetBackingRef() const;

    protected:
    void LogError(const Worker& worker, std::string function_name, std::string error_message, int cl_error_code) const;
    void LogError(const Worker& worker, std::string function_name, std::string error_message) const;

    bool valid_ = false;
    const Manager& manager_;
    std::unique_ptr<cl::Buffer> buffer_;
    size_t size_;
};

class BufferException : public OpenCLException {
    public:
    BufferException(const std::string& message, int cl_error_code) : OpenCLException(message, cl_error_code) {}
};

} // namespace OpenCL

} // namespace Re

#endif // RE_OPENCL_GENERICBUFFER_H