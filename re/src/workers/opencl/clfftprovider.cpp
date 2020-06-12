#include "utilities.h"

#include "clfftprovider.h"
#include "manager.h"

#include <clFFT.h>

using namespace Re::OpenCL;

template<> CLFFTProvider* Re::OpenCL::make_provider<CLFFTProvider>(const Manager& manager)
{
    return new CLFFTProvider(manager);
}

CLFFTProvider::CLFFTProvider(const Manager& manager) :
    is_valid_(true),
    manager_(manager),
    fft_setup_data_(std::make_unique<clfftSetupData>())
{
    clfftStatus err = clfftInitSetupData(fft_setup_data_.get());
    if(err != clfftStatus::CLFFT_SUCCESS) {
        throw CLFFTException("Unable to successfully initialise clFFT setup data", err);
    }
    err = clfftSetup(fft_setup_data_.get());
    if(err != clfftStatus::CLFFT_SUCCESS) {
        throw CLFFTException("Unable to successfully set up the clFFT library", err);
    }
}

CLFFTProvider::~CLFFTProvider()
{
    if(is_valid_) {
        try {
            Teardown();
        } catch(...) {
            // Discard exceptions to satisfy Destructor noexcept specifier
        }
    }
};

void CLFFTProvider::Teardown()
{
    if(!is_valid_) {
        throw InvalidCLFFTProviderException("Attempting to teardown");
    }

    clfftStatus err = clfftTeardown();
    if(err != clfftStatus::CLFFT_SUCCESS) {
        throw CLFFTException("Could not successfully tear down clFFT library", err);
    }
}

std::vector<float> CLFFTProvider::FFT(std::vector<float>& data, Device& device)
{
    if(!is_valid_) {
        throw InvalidCLFFTProviderException("Unable to perform FFT calculation on vector");
    }

    /* Prepare OpenCL memory objects and place data inside them. */
    std::optional<Buffer<float>> buffer;
    try {
        buffer = manager_.CreateBuffer(data, device, true);
    } catch(const std::exception& e) {
        throw std::runtime_error(std::string("CLFFTProvider unable to perform FFT on vector:\n") + e.what());
    }

    std::optional<Buffer<float>> out_buffer;
    try {
        out_buffer = FFT(buffer.value(), device);
    } catch(const std::exception& e) {
        throw std::runtime_error(std::string("CLFFT Provider unable to perform FFT on buffer then read:\n") + e.what());
    }

    try {
        /* Fetch results of calculations. */
        auto& actual_buffer = out_buffer.value();
        return actual_buffer.ReadData(device, true);
    } catch(const std::exception& e) {
        throw std::runtime_error(std::string("CLFFT Provider unable to read result from FFT buffer:\n") + e.what());
    }
}

Buffer<float> CLFFTProvider::FFT(Buffer<float>& buffer, Device& device)
{
    if(!is_valid_) {
        throw InvalidCLFFTProviderException("Unable to perform FFT calculation on buffer");
    }

    size_t length = buffer.GetNumElements();

    if(length % 2 != 0) {
        throw CLFFTException(
            "Unable to perform FFT: Non-even length of array, implies half a sample (No complex number)",
            CLFFT_INVALID_BUFFER_SIZE);
    }

    // Find the size of each datapoint and how many datapoints there are
    size_t N = length / 2;

    // Check that prime decomposition only uses 2, 3 and 5 as clFFT doesnt support other prime numbers
    if(N == 0) {
        throw CLFFTException("Unable to perform FFT: Passed an array of length 0", CLFFT_INVALID_BUFFER_SIZE);
    }
    while(N % 2 == 0)
        N = N / 2;
    while(N % 3 == 0)
        N = N / 3;
    while(N % 5 == 0)
        N = N / 5;
    if(N > 1) {
        throw CLFFTException("Unable to perform FFT: Number of samples must be a mix of powers of 2, 3 and 5",
                             CLFFT_INVALID_BUFFER_SIZE);
    }
    N = length;

    /* FFT library realted declarations */
    clfftDim dim = CLFFT_1D;
    size_t clLengths[1] = {N};

    /* Create a default plan for a complex FFT. */
    clfftStatus status;
    clfftPlanHandle planHandle;
    status = clfftCreateDefaultPlan(&planHandle, manager_.GetContext()(), dim, clLengths);
    if(status != CLFFT_SUCCESS) {
        throw CLFFTException("Failed to create default plan while performing FFT", status);
    }

    /* Set plan parameters. */
    status = clfftSetPlanPrecision(planHandle, CLFFT_SINGLE);
    if(status != CLFFT_SUCCESS) {
        throw CLFFTException("Failed to set plan precision to single while performing FFT", status);
    }
    // err = clfftSetLayout(planHandle, CLFFT_COMPLEX_INTERLEAVED, CLFFT_COMPLEX_INTERLEAVED);
    // err = clfftSetLayout(planHandle, CLFFT_COMPLEX_INTERLEAVED, CLFFT_COMPLEX_PLANAR);
    status = clfftSetLayout(planHandle, CLFFT_REAL, CLFFT_HERMITIAN_INTERLEAVED);
    if(status != CLFFT_SUCCESS) {
        throw CLFFTException("Failed to set plan layout to Real Hermetian while performing FFT", status);
    }
    status = clfftSetResultLocation(planHandle, CLFFT_INPLACE);
    if(status != CLFFT_SUCCESS) {
        throw CLFFTException("Failed to plan result location to be in-place while performing FFT", status);
    }

    cl_command_queue dev_queue = device.GetQueue().GetRef()();
    /* Bake the plan. */
    status = clfftBakePlan(planHandle, 1, &dev_queue, NULL, NULL);
    if(status != CLFFT_SUCCESS) {
        throw CLFFTException("Failed to bake the plan while performing FFT", status);
    }

    /* Execute the plan. */
    status = clfftEnqueueTransform(planHandle, CLFFT_FORWARD, 1, &dev_queue, 0, NULL, NULL, &(buffer.GetBackingRef()()),
                                   NULL, NULL);
    if(status != CLFFT_SUCCESS) {
        throw CLFFTException("Failed to enqueue the transform while performing FFT", status);
    }

    /* Wait for calculations to be finished. */
    cl_int cl_err = device.GetQueue().GetRef().finish();
    if(cl_err != CLFFT_SUCCESS) {
        throw OpenCLException("Failed to wait for transform to finish while performing FFT", cl_err);
    }

    /* Release the plan. */
    status = clfftDestroyPlan(&planHandle);
    if(status != CLFFT_SUCCESS) {
        throw CLFFTException("Failed to destroy the plan while performing FFT", status);
    }

    return std::move(buffer);
}

CLFFTException::CLFFTException(const std::string& message, int clfft_error_code) :
    OpenCLException(message, clfft_error_code){};