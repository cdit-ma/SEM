#ifndef RE_OPENCL_KERNEL_H
#define RE_OPENCL_KERNEL_H

typedef struct _cl_mem* cl_mem;

#include "bufferbase.h"
#include "kernelbase.h"
#include "openclexception.h"

namespace Re {

namespace OpenCL {

class Kernel : public KernelBase {
    public:
    Kernel(Manager& manager, cl::Kernel& kernel);

    template<typename T0, typename... Ts> void SetArgs(T0& arg0, Ts&... args);

    protected:
    virtual void
    LogError(const Worker& worker, std::string function_name, std::string error_message, int cl_error_code);
    virtual void LogError(const Worker& worker, std::string function_name, std::string error_message);

    private:
    template<typename T0, typename... Ts> void SetArgsRecursive(unsigned int index, T0& arg0, Ts&... args);
    void SetArgsRecursive(unsigned int index, cl::LocalSpaceArg& local_space) { SetArg(index, local_space); }
    template<typename T0>
    typename std::enable_if<std::is_base_of<cl::Memory, T0>::value, void>::type SetArgsRecursive(unsigned int index,
                                                                                                 T0& arg0);
    template<typename T0>
    typename std::enable_if<std::is_base_of<BufferBase, T0>::value, void>::type SetArgsRecursive(unsigned int index,
                                                                                                 T0& arg0);
    template<typename T0>
    typename std::enable_if<!std::is_base_of<cl::Memory, T0>::value && !std::is_base_of<BufferBase, T0>::value,
                            void>::type
    SetArgsRecursive(unsigned int index, T0& arg0);
    template<typename T0>
    typename std::enable_if<std::is_pointer<T0>::value, void>::type setGenericArg(unsigned int index, T0& arg);
    template<typename T0>
    typename std::enable_if<!std::is_pointer<T0>::value, void>::type setGenericArg(unsigned int index, T0& arg);
};

template<typename T0, typename... Ts> void Kernel::SetArgs(T0& arg0, Ts&... args)
{
    std::lock_guard<std::mutex> guard(kernel_mutex_);
    SetArgsRecursive(0, arg0, args...);
}

template<typename T0, typename... Ts> void Kernel::SetArgsRecursive(unsigned int index, T0& arg0, Ts&... args)
{
    static_assert(!std::is_pointer<T0>::value, "SetArgs does not accept pointered types");
    // Call base function with to set current argument
    SetArgsRecursive(index, arg0);
    // Handle the rest of the arguments
    SetArgsRecursive(index + 1, args...);
}

template<typename T0>
typename std::enable_if<std::is_base_of<cl::Memory, T0>::value, void>::type Kernel::SetArgsRecursive(unsigned int index,
                                                                                                     T0& arg0)
{
    SetArg(index, arg0);
}

template<typename T0>
typename std::enable_if<std::is_base_of<BufferBase, T0>::value, void>::type Kernel::SetArgsRecursive(unsigned int index,
                                                                                                     T0& arg0)
{
    const cl::Buffer& buffer_ref = static_cast<const BufferBase&>(arg0).GetBackingRef();
    SetArg(index, buffer_ref);
}

template<typename T0>
typename std::enable_if<!std::is_base_of<cl::Memory, T0>::value && !std::is_base_of<BufferBase, T0>::value, void>::type
Kernel::SetArgsRecursive(unsigned int index, T0& arg0)
{
    return setGenericArg(index, arg0);
}

template<typename T0>
typename std::enable_if<std::is_pointer<T0>::value, void>::type Kernel::setGenericArg(unsigned int index, T0& arg)
{
    return SetArg(index, sizeof(std::remove_reference<T0>::type), arg);
}

template<typename T0>
typename std::enable_if<!std::is_pointer<T0>::value, void>::type Kernel::setGenericArg(unsigned int index, T0& arg)
{
    return SetArg(index, sizeof(T0), &arg);
}

} // namespace OpenCL

} // namespace Re

#endif // RE_OPENCL_KERNEL_H