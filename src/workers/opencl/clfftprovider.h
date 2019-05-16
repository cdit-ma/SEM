#ifndef RE_OPENCL_CLFFTPROVIDER_H
#define RE_OPENCL_CLFFTPROVIDER_H

#include "fftprovider.h"

struct clfftSetupData_;

namespace Re {
namespace OpenCL {

class CLFFTProvider : public FFTProvider {
    public:
    CLFFTProvider(const Manager& manager);
    virtual ~CLFFTProvider();

    /**
     * Allows the teardown function of the clFFT library to be called
     * to facilitate the explicit handling of exceptions/errors
     **/
    void Teardown() override;

    virtual std::vector<float> FFT(std::vector<float>& data, Device& device) override;
    virtual Buffer<float> FFT(Buffer<float>& data, Device& device) override;

    private:
    bool is_valid_;
    const Manager& manager_;
    std::unique_ptr<clfftSetupData_> fft_setup_data_;
};

template<> CLFFTProvider* make_provider<CLFFTProvider>(const Manager& manager);

class CLFFTException : public OpenCLException {
    public:
    CLFFTException(const std::string& message, int clfft_error_code);
};

class InvalidCLFFTProviderException : public std::runtime_error {
    public:
    InvalidCLFFTProviderException(const std::string& error_string) : std::runtime_error(error_string){};
};

} // namespace OpenCL

} // namespace Re

#endif // RE_OPENCL_CLFFTPROVIDER_H
