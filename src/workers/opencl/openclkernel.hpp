#ifndef OPENCLKERNEL_H
#define OPENCLKERNEL_H

//#define __CL_ENABLE_EXCEPTIONS

#include "openclkernelbase.h"
#include "genericbuffer.h"

class OpenCLKernel : public OpenCLKernelBase {
public:
    OpenCLKernel(OpenCLManager& manager, cl::Kernel& kernel, Worker* worker);

    template <typename T0, typename... Ts>
    bool SetArgs(T0& arg0, Ts&... args);

protected:
    virtual void LogError(std::string function_name, std::string error_message, cl_int cl_error_code);
    virtual void LogError(std::string function_name, std::string error_message);

private:
    template <typename T0, typename... Ts>
    bool SetArgsRecursive(unsigned int index, T0& arg0, Ts& ... args);
    template <typename T0>
    //typename std::enable_if<!std::is_base_of<GenericBuffer, T0>::value>::type
    bool SetArgsRecursive(unsigned int index, T0& arg0);
    
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

template <typename T0>
//typename std::enable_if<!std::is_base_of<GenericBuffer, T0>::value>::type
bool OpenCLKernel::SetArgsRecursive(unsigned int index, T0& arg0) {
    static_assert(!std::is_pointer<T0>::value, "SetArgs does not accept pointered types");
    //static_assert(!std::is_base_of<GenericBuffer, T0>::value, "GenericBuffer in the generic arg setter!");
    //std::cerr << "entering base call for type " << typeid(T0).name() <<std::endl;

    cl_int err;

    // If we are passed something derived from GenericBuffer we know the backing reference will be properly handled
    if(std::is_base_of<GenericBuffer, T0>::value) {
        const cl::Buffer& buffer_arg = reinterpret_cast<const GenericBuffer*>(&arg0)->GetBackingRef();
        std::cout << "Bakcing ref in setArgs at index " << index << ": " << &(buffer_arg) << std::endl;
        err = kernel_.setArg(index, buffer_arg);
    } else {
        err = kernel_.setArg(index, arg0);
    }

    if (err != CL_SUCCESS) {
        LogError(__func__,
            "Unable to set parameter "+std::to_string(index)+" of a kernel",
            err);
            return false;
    }
    //std::cerr << "set arg " << index << std::endl;
    return true;
}


/*template <typename T0>
void OpenCLKernel::SetArgsRecursive(unsigned int index, GenericBuffer& arg0) {
    std::cerr << "entering specialised buffer base call" << std::endl;
    SetArgsRecursive(index, arg0.GetBackingRef());
}*/
#endif