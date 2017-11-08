#ifndef OPENCLKERNEL_H
#define OPENCLKERNEL_H

//#define __CL_ENABLE_EXCEPTIONS

typedef struct _cl_mem *cl_mem;

#include "openclkernelbase.h"
#include "genericbuffer.h"

//#include "openclutilities.h"

class OpenCLKernel : public OpenCLKernelBase {
public:
    OpenCLKernel(OpenCLManager& manager, cl::Kernel& kernel, Worker* worker);

    template <typename T0, typename... Ts>
    bool SetArgs(T0& arg0, Ts&... args);

protected:
    virtual void LogError(std::string function_name, std::string error_message, int cl_error_code);
    virtual void LogError(std::string function_name, std::string error_message);

private:
    template <typename T0, typename... Ts>
    bool SetArgsRecursive(unsigned int index, T0& arg0, Ts& ... args);
    //template <typename T0, typename T0>
    //typename std::enable_if<!std::is_base_of<GenericBuffer, T0>::value>::type
    bool SetArgsRecursive(unsigned int index, cl::LocalSpaceArg& local_space) {
        return SetArg(index, local_space);
    }
    template <typename T0>
    typename std::enable_if<std::is_base_of<cl::Memory, T0>::value, bool>::type 
    SetArgsRecursive(unsigned int index, T0& arg0);
    template <typename T0>
    typename std::enable_if<std::is_base_of<GenericBuffer, T0>::value, bool>::type 
    SetArgsRecursive(unsigned int index, T0& arg0);
    template <typename T0>
    typename std::enable_if<!std::is_base_of<cl::Memory, T0>::value && !std::is_base_of<GenericBuffer, T0>::value , bool>::type 
    SetArgsRecursive(unsigned int index, T0& arg0);
    /*template <typename T0, typename std::enable_if<std::is_base_of<cl::Memory, T0>::value, T0>::type>
    bool SetArgsRecursive(unsigned int index, T0& arg0);*/
    template <typename T0>
    typename std::enable_if<std::is_pointer<T0>::value, bool>::type
    setGenericArg(unsigned int index, T0& arg);
    template <typename T0>
    typename std::enable_if<!std::is_pointer<T0>::value, bool>::type
    setGenericArg(unsigned int index, T0& arg);
    
};

template <typename T0, typename... Ts>
bool OpenCLKernel::SetArgs(T0& arg0, Ts&... args) {
    // std::cerr << "about to make the recursive call... " <<std::endl;
    return SetArgsRecursive(0, arg0, args...);
}

template <typename T0, typename... Ts>
bool OpenCLKernel::SetArgsRecursive(unsigned int index, T0& arg0, Ts& ... args) {
    static_assert(!std::is_pointer<T0>::value, "SetArgs does not accept pointered types");
    //std::cerr << "entering recursive call " <<std::endl;
    // Call base function with to set current argument
    bool success = SetArgsRecursive(index, arg0);
    if (!success) {
        return false;
    }
    // Handle the rest of the arguments
    return SetArgsRecursive(index+1, args...);
}

/*template <typename T0>
//typename std::enable_if<!std::is_base_of<GenericBuffer, T0>::value>::type
bool OpenCLKernel::SetArgsRecursive(unsigned int index, T0& arg0) {
    static_assert(!std::is_pointer<T0>::value, "SetArgs does not accept pointered types");
    //static_assert(!std::is_base_of<GenericBuffer, T0>::value, "GenericBuffer in the generic arg setter!");
    //std::cerr << "entering base call for type " << typeid(T0).name() <<std::endl;

    cl_int err;

    // If we are passed something derived from GenericBuffer we know the backing reference will be properly handled
    if(std::is_base_of<GenericBuffer, T0>::value) {
        const cl::Buffer& buffer_arg = reinterpret_cast<const GenericBuffer*>(&arg0)->GetBackingRef();
        err = kernel_->setArg(index, buffer_arg);
    } else {
        err = kernel_->setArg(index, arg0);
    }

    if (err != CL_SUCCESS) {
        LogError(__func__,
            "Unable to set parameter "+std::to_string(index)+" of a kernel",
            err);
        return false;
    }
    //std::cerr << "set arg " << index << std::endl;
    return true;
}*/

template <typename T0>
typename std::enable_if<std::is_base_of<cl::Memory, T0>::value, bool>::type
OpenCLKernel::SetArgsRecursive(unsigned int index, T0& arg0) {
    //std::cerr << "entering base call for memory object of type " << typeid(T0).name() <<std::endl;
    bool success = SetArg(index, arg0);
    
    if (!success) {
        LogError(__func__,
            "Unable to set parameter "+std::to_string(index)+" of a kernel");
        return false;
    }
    //std::cerr << "set arg " << index << std::endl;
    return true;
}

template <typename T0>
typename std::enable_if<std::is_base_of<GenericBuffer, T0>::value, bool>::type
OpenCLKernel::SetArgsRecursive(unsigned int index, T0& arg0) {
    //std::cerr << "entering base call for GenericBuffer object of type " << typeid(T0).name() <<std::endl;
    const cl::Buffer& buffer_ref = static_cast<const GenericBuffer*>(&arg0)->GetBackingRef();
    bool success = SetArg(index, buffer_ref);
    
    if (!success) {
        LogError(__func__, "Unable to set parameter "+std::to_string(index)+" of a kernel");
        return false;
    }
    //std::cerr << "set arg " << index << std::endl;
    return true;
}

template <typename T0>
typename std::enable_if<!std::is_base_of<cl::Memory, T0>::value && !std::is_base_of<GenericBuffer, T0>::value , bool>::type
OpenCLKernel::SetArgsRecursive(unsigned int index, T0& arg0) {
    //std::cerr << "entering base call for regular object of type " << typeid(T0).name() <<std::endl;
    //return SetArg(index, sizeof(T0), arg0);
    return setGenericArg(index, arg0);
}

template <typename T0>
typename std::enable_if<std::is_pointer<T0>::value, bool>::type
OpenCLKernel::setGenericArg(unsigned int index, T0& arg) {
    return SetArg(index, sizeof(std::remove_reference<T0>::type), arg);
}

template <typename T0>
typename std::enable_if<!std::is_pointer<T0>::value, bool>::type
OpenCLKernel::setGenericArg(unsigned int index, T0& arg) {
    return SetArg(index, sizeof(T0), &arg);
}


#endif