#ifndef GENERICBUFFER_H
#define GENERICBUFFER_H

#include "openclutilities.h"

class OpenCLManager;

class GenericBuffer {
public:
    GenericBuffer(OpenCLManager* manager, size_t size);
    
    bool is_valid() const;
    size_t GetSize() const;
    int GetID() const;
    
    const cl::Buffer& GetBackingRef() const;

protected:
    int id_ = -1;
    bool valid_ = false;
    OpenCLManager* manager_;
    cl::Buffer buffer_;
    size_t size_;
};

#endif