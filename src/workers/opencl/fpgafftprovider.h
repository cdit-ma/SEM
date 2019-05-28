#ifndef RE_OPENCL_FPGAFFTPROVIDER_H
#define RE_OPENCL_FPGAFFTPROVIDER_H

#include "fftprovider.h"

namespace Re::OpenCL {

class FPGAFFTProvider : public FFTProvider {
    public:
    FPGAFFTProvider(const Manager& manager);

    void Teardown() override;

    virtual std::vector<float> FFT(std::vector<float>&, Device& device) override;
    virtual Buffer<float> FFT(Buffer<float>& buffer, Device& device) override;

    private:
    const Manager& manager_;

    Buffer<float> RunKernels(Buffer<float>& buffer, Device& device);
    Kernel& GetKernel(Device& device, const std::string& kernel_name, const std::string& source_file);
};


class FPGAFFTException : public std::runtime_error {
    public:
    FPGAFFTException(const std::string& err_msg) : std::runtime_error(err_msg) {}
};

} // namespace Re::OpenCL

#endif // RE_OPENCL_FPGAFFTPROVIDER