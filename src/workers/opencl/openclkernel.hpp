#ifndef OPENCLKERNEL_H
#define OPENCLKERNEL_H

//#define __CL_ENABLE_EXCEPTIONS

#include "openclkernelbase.h"
#include "genericbuffer.h"

class OpenCLKernel : public OpenCLKernelBase {
public:
    OpenCLKernel(OpenCLManager& manager, cl::Kernel& kernel, Worker* worker);

    template <typename T0, typename... Ts>
    void SetArgs(T0 arg0, Ts... args);

private:
    
    template <typename T0, typename... Ts>
    void SetArgsRecursive(unsigned int index, T0& arg0, Ts& ... args);
    template <typename T0>
    //typename std::enable_if<!std::is_base_of<GenericBuffer, T0>::value>::type
    void SetArgsRecursive(unsigned int index, T0& arg0);
    /*template <typename T0>
    void SetArgsRecursive(unsigned int index, GenericBuffer& arg0);*/

    void LogError(std::string function_name, std::string error_message, cl_int cl_error_code);
    void LogError(std::string function_name, std::string error_message);

    // template <typename BT>
    // cl::Kernel StealBackingReference(OCLBuffer<BT>& buffer);
    
};

template <typename T0, typename... Ts>
void OpenCLKernel::SetArgs(T0 arg0, Ts... args) {
    // std::cerr << "about to make the recursive call... " <<std::endl;
    SetArgsRecursive(0, arg0, args...);
}

template <typename T0, typename... Ts>
void OpenCLKernel::SetArgsRecursive(unsigned int index, T0& arg0, Ts& ... args) {
    static_assert(!std::is_pointer<T0>::value, "SetArgs does not accept pointered types");
    //std::cerr << "entering recursive call " <<std::endl;
    // Call base function with to set current argument
    SetArgsRecursive(index, arg0);
    // Handle the rest of the arguments
    SetArgsRecursive(index+1, args...);
}

template <typename T0>
//typename std::enable_if<!std::is_base_of<GenericBuffer, T0>::value>::type
void OpenCLKernel::SetArgsRecursive(unsigned int index, T0& arg0) {
    static_assert(!std::is_pointer<T0>::value, "SetArgs does not accept pointered types");
    //static_assert(!std::is_base_of<GenericBuffer, T0>::value, "GenericBuffer in the generic arg setter!");
    //std::cerr << "entering base call for type " << typeid(T0).name() <<std::endl;

    cl_int err;

    // If we are passed something derived from GenericBuffer we know the backing reference will be properly handled
    if(std::is_base_of<GenericBuffer, T0>::value) {
        err = kernel_.setArg(index, reinterpret_cast<GenericBuffer*>(&arg0)->GetBackingRef());
    } else {
        err = kernel_.setArg(index, arg0);
    }

    if (err != CL_SUCCESS) {
        LogError(__func__,
            "Unable to set parameter "+std::to_string(index)+" of a kernel",
            err);
    }
    //std::cerr << "set arg " << index << std::endl;
}


/*template <typename T0>
void OpenCLKernel::SetArgsRecursive(unsigned int index, GenericBuffer& arg0) {
    std::cerr << "entering specialised buffer base call" << std::endl;
    SetArgsRecursive(index, arg0.GetBackingRef());
}*/
#endif