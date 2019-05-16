#include "utilities.h"

#include "fpgafftprovider.h"

#include "fft1d/aocl_utils.h"
#include "fft1d/fft_config.h"

#include "manager.h"

// the above header defines log of the FFT size hardcoded in the kernel
// compute N as 2^LOGN
#define N (1 << LOGN)

// Account for changes in macro names throughout SDK ownership transitions
#ifdef CL_MEM_BANK_2_ALTERA
#define CHANNEL_2 CL_MEM_BANK_ALTERA
#else
#define CHANNEL_2 CL_CHANNEL_2_INTELFPGA
#endif // CL_MEM_BANK_2_ALTERA

using namespace Re::OpenCL;

template<> FPGAFFTProvider* Re::OpenCL::make_provider<FPGAFFTProvider>(const Manager& manager)
{
    return new FPGAFFTProvider(manager);
}

// FFT operates with complex numbers - store them in a struct
typedef struct {
    double x;
    double y;
} double2;

typedef struct {
    float x;
    float y;
} float2;

namespace detail {
Queue& GetOrCreateFetchQueue(Device& device)
{
    try {
        return device.GetQueue("fetch");
    } catch(const std::runtime_error& re) {
        return device.AddQueue("fetch");
    }
};

template<typename T> std::vector<T> TryReadBuffer(Buffer<T>& buffer, Device& device, const std::string& error_msg)
{
    try {
        return buffer.ReadData(device, true);
    } catch(const OpenCLException& e) {
        throw OpenCLException(error_msg + "\n" + e.what(), e.ErrorCode());
    }
};

// TODO: Create an in-place version for greater efficiency
void bit_reverse(float2* src, float2* dst, size_t num_elements)
{
    int bit_length = LOGN - 1;

    /* Re-enable for dynamically sized reversal
    int bit_length = 0;
    while (1 << bit_length < num_elements) bit_length++;
    bit_length -= 1;
    */

    for(unsigned int index = 0; index < num_elements; index++) {
        unsigned int norm_index = index;
        unsigned int br_index = index & 1;
        // int s = sizeof(norm_index) * CHAR_BIT - 1;
        int s = bit_length;
        for(norm_index >>= 1; norm_index; norm_index >>= 1) {
            br_index <<= 1;
            br_index |= (norm_index & 1);
            s--;
        }
        br_index <<= s;

        dst[br_index].x = src[index].x;
        dst[br_index].y = src[index].y;
    }
}

} // namespace detail

FPGAFFTProvider::FPGAFFTProvider(const Manager& manager) : manager_(manager) {}

Buffer<float> FPGAFFTProvider::FFT(Buffer<float>& data, Device& device)
{
    auto&& out_buffer = RunKernels(data, device);

    auto intermediate_data =
        detail::TryReadBuffer(out_buffer, device, "Failed to read buffer back after performing FFT");

    std::vector<float> out_data(intermediate_data.size());

    // Altera FPGA implementation leaves out the bit reversal, so it needs to be performed now on the CPU
    detail::bit_reverse((float2*)intermediate_data.data(), (float2*)out_data.data(), out_data.size() / 2);

    return manager_.CreateBuffer(out_data, device, true);
}

std::vector<float> FPGAFFTProvider::FFT(std::vector<float>& data, Device& device)
{
    auto in_buffer = manager_.CreateBuffer(data, device, true);

    auto&& out_buffer = RunKernels(in_buffer, device);

    auto intermediate_data =
        detail::TryReadBuffer(out_buffer, device, "Failed to read buffer back after performing FFT");

    std::vector<float> out_data(intermediate_data.size());

    // Altera FPGA implementation leaves out the bit reversal, so it needs to be performed now on the CPU
    detail::bit_reverse((float2*)intermediate_data.data(), (float2*)out_data.data(), out_data.size() / 2);

    return out_data;
}

void FPGAFFTProvider::Teardown() {}

/*
std::vector<float> OpenCL_Worker::FFT(Buffer<float>& buffer, Device& device)
{
    Log(GET_FUNC, Logger::WorkloadEvent::ERROR, work_id,
        "Unable to run buffered FFT on FPGA, consider using the vector input version instead");
    return false;
}

bool OpenCL_Worker::InitFFT(int work_id)
{
    cl_int status;

    // Loop through all of the devices available to the worker for initialisation
    for(auto& d_ref : devices_) {
        auto& device = d_ref.get();

        // Create the fetch queue.
        fetch_queues_.emplace_back(*manager_, device);

        // Check if both required fft1d and fetch kernels are in the list of available kernels
        int kernels_found = 0;
        for(auto& ref_wrapper : device.GetKernels()) {
            auto& kernel_ = ref_wrapper.get();
            if(kernel_.GetName() == "fft1d" || kernel_.GetName() == "fetch") {
                kernels_found++;
            }
        }

        // Recreate the program from a binary if either kernel isn't present
        if(kernels_found < 2) {
            bool did_read_binary = device.LoadKernelsFromBinary(*this, "fft1d.aocx");
            if(!did_read_binary) {
                Log(__func__, Logger::WorkloadEvent::ERROR, work_id, "Unable to load kernels from fft1d.aocx binary");
                return false;
            }
        }

        // Create the kernel - name passed in here must match kernel name in the
        // original CL file, that was compiled into an AOCX file using the AOC tool
        auto& kernels = device.GetKernels();
        for(OpenCLKernel& k : kernels) {
            if(k.GetName() == "fft1d") {
                fpga_fft_kernel_ = &k;
            }
            if(k.GetName() == "fetch") {
                fpga_fetch_kernel_ = &k;
            }
        }
    }

    return true;
}
*/

Buffer<float> FPGAFFTProvider::RunKernels(Buffer<float>& data, Device& device)
{
    cl_int status = 0;

    // Altera implementation requires exactly N complex numbers
    if(data.GetNumElements() != N * 2) {
        throw FPGAFFTExcecption("FFT implementation for FPGA requires data of length " + std::to_string(N)
                                + ", skipping calculation");
    }

    /*
    size_t data_size = sizeof(float2) * data.size() / 2;
    float2* h_inData = (float2*)aocl_utils::alignedMalloc(data_size);

    // For 0 padding if sizes < N are to be supported with fixed values of LOGN
    memset(h_inData, 0, data_size);

    float2* h_outData = (float2*)aocl_utils::alignedMalloc(data_size);
    memcpy(h_inData, data.data(), data_size);
    */

    // Create device buffers - assign the buffers in different banks for more efficient
    // memory access
    /*
    cl::Buffer d_inData(manager_->GetContext(), CL_MEM_READ_WRITE, data_size, NULL, &status);
    aocl_utils::checkError(status, "Failed to allocate input device buffer\n");
    */
    Buffer<float> d_outData(manager_, data.GetByteSize(), CL_MEM_READ_WRITE | CHANNEL_2);
    /*cl::Buffer d_outData(manager_.GetContext(), CL_MEM_READ_WRITE | CHANNEL_2, data.GetByteSize(), NULL,
    &status);
    if(status != CL_SUCCESS) {
        throw OpenCLException("Failed to allocate output device buffer", status);
    }*/
    // Buffer intermediate_data(d_outData);
    // aocl_utils::checkError(status, "Failed to allocate output device buffer\n");

    auto& send_queue = device.GetQueue().GetRef();
    auto& fetch_queue = detail::GetOrCreateFetchQueue(device);

    // Copy data from host to device
    /*
    status = send_queue.enqueueWriteBuffer(d_inData, CL_TRUE, 0, data_size, h_inData, NULL, NULL);
    aocl_utils::checkError(status, "Failed to copy data to device");
    */

    // Can't pass bool to device, so convert it to int
    int inverse_int = 0; // false
    int iterations = 1;

    // Take ownership of kernels associated with FFTs on that device
    auto& fft_kernel = GetKernel(device, "fft1d", "precompiled.aocx");
    auto fft_kernel_lock = fft_kernel.AcquireLock();
    auto& fetch_kernel = GetKernel(device, "fetch", "precompiled.aocx");
    auto fetch_kernel_lock = fetch_kernel.AcquireLock();

    // Set the kernel arguments
    fetch_kernel.SetArgs(data);
    fft_kernel.SetArgs(d_outData, iterations, inverse_int);

    // Queue the FFT task
    status = send_queue.enqueueTask(fft_kernel.GetBackingRef(), NULL, NULL);
    // fft_kernel.Run(device, false, cl::NullRange, cl::NDRange(1), cl::NDRange(1));
    aocl_utils::checkError(status, "Failed to launch kernel");

    size_t ls = N / 8;
    cl::NDRange fetch_local_range(ls);
    size_t gs = iterations * ls;
    cl::NDRange fetch_global_range(gs);
    status = fetch_queue.GetRef().enqueueNDRangeKernel(fetch_kernel.GetBackingRef(), cl::NullRange, fetch_global_range,
                                                       fetch_local_range);
    aocl_utils::checkError(status, "Failed to launch fetch kernel");

    // Wait for command queue to complete pending events
    status = send_queue.finish();
    aocl_utils::checkError(status, "Failed to finish");

    status = fetch_queue.GetRef().finish();
    aocl_utils::checkError(status, "Failed to finish queue1");

    fft_kernel_lock.unlock();
    fetch_kernel_lock.unlock();

    return d_outData;
}

Kernel& FPGAFFTProvider::GetKernel(Device& device, const std::string& kernel_name, const std::string& source_file)
{
    try {
        // First check list list of already compiled kernels
        for(Kernel& kernel : device.GetKernels()) {
            if(kernel.GetName() == kernel_name) {
                return kernel;
            }
        }

        // If none were found then attempt to freshly load from source code
        // std::vector<std::string> filenames;
        // filenames.push_back(source_file);
        device.LoadKernelsFromBinary(source_file);

        auto kernel_vec = device.GetKernels();
        if(kernel_vec.size() == 0) {
            /*Log(__func__, Logger::WorkloadEvent::ERROR, get_new_work_id(),
            "Unable to find any kernels in "+source_file);*/
            throw std::runtime_error("No kernels in file " + source_file);
        }

        for(Kernel& kernel : kernel_vec) {
            if(kernel.GetName() == kernel_name) {
                return kernel;
            }
        }

        /*Log(__func__, Logger::WorkloadEvent::MESSAGE, get_new_work_id(),
            "Unable to find a kernel called "+kernel_name+" in "+source_file);*/
        throw std::runtime_error("Unable to find kernel with name " + kernel_name + " in file " + source_file);
    } catch(const std::exception& e) {
        throw OpenCLException("Unable to retrieve kernel " + kernel_name + " for device " + device.GetName() + ":\n"
                                  + e.what(),
                              CL_INVALID_KERNEL);
    }
}