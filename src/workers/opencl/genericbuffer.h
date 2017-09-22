#ifndef GENERICBUFFER_H
#define GENERICBUFFER_H

#include "openclutilities.h"

class GenericBuffer {
public:
    GenericBuffer(int id);
    
    bool is_valid();
    
    int GetID() const;
    
    const cl::Buffer& GetBackingRef() const;

protected:
    int id_ = -1;
    bool valid_ = false;
    cl::Buffer buffer_;
};

#endif