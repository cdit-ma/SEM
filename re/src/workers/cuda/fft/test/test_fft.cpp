//
// Created by Jackson Michael on 1/5/20.
//

#include <gtest/gtest.h>

#include <cmath>
#include <cuffthandle.cuh>

#include <memory>
#include <utility>

namespace cditma::cuda::test {

/**
 * Base class that provides a convenient interface for associating test input data with an expected
 * output
 * @tparam InputType The exact data required to be given to the test, may be a complex struct/object
 * in order to convey all of the necessary information
 * @tparam OutputType Similar to the input type, however this structure will contain all of the
 * information that the test needs to see if the result produced by the test is whatis expected
 */
template<typename InputType, typename OutputType> class TestPair {
public:
    /**
     * Each TestPair is required to have the fully formed input and output data from the moment it
     * is constructed
     * @param input The complete information required to run a given test
     * @param expected_output The complete understanding of the relevant data to be checked against
     * upon test completion
     */
    TestPair(InputType input, OutputType expected_output) :
        input_(std::move(input)), output_(std::move(expected_output)){};

    /**
     * Gives access to the input data required for a given test
     * @return A copy of the input data with which the test pair was created
     */
    [[nodiscard]] auto GetInput() const -> InputType { return input_; };
    /**
     * Gives access to the output data required for a given test
     * @return A copy of the output data with which the test pair was created
     */
    [[nodiscard]] auto GetExpectedOutput() const -> OutputType { return output_; };

private:
    InputType input_;
    OutputType output_;
};

/**
 * A class that associates a given input vector with the expected output for the FFT operation
 * performed on it.
 * @tparam ElemType The type of primitive data being operated on (float, integer etc.)
 */
template<typename ElemType>
class FFTTestPair : public TestPair<CUFFTVec<ElemType>, CUFFTVec<ElemType>> {
public:
    /**
     * Constructs an FFT test pair for a sine wave with the given frequency and amplitude, where the
     * number of samples in the array is equal to 2*length (doubled due to the need for complex
     * numbers)
     * @param length The amount of samples the input data should represent
     * @param frequency The frequency of the sine wave in Hz
     * @param amplitude
     */
    FFTTestPair(size_t length, float frequency, float amplitude, float phase_shift) :
        TestPair<CUFFTVec<ElemType>, CUFFTVec<ElemType>>(
            GenerateInputWaveData(length, frequency, amplitude, phase_shift),
            GenerateOutputWaveData(length, frequency, amplitude, phase_shift)){};

private:
    /**
     * Generate the discrete sample data for a sine wave in the time domain.
     * @param num_samples The number of samples to produce
     * @param frequency The frequency of the wave in Hz
     * @param amplitude The amplitude of the wave
     * @param phase_shift The phase shift of the sine wave
     * @return A vector of ElemType, with length double that of the number of samples
     */
    static auto
    GenerateInputWaveData(size_t num_samples, float frequency, float amplitude, float phase_shift)
        -> CUFFTVec<ElemType>;
    /**
     * Generate the discrete sample data for a sine wave in the frequency domain.
     * @param num_samples The number of samples to produce
     * @param frequency The frequency of the wave in Hz
     * @param amplitude The amplitude of the wave
     * @param phase_shift The phase shift of the sine wave
     * @return A vector of ElemType, with length double that of the number of samples
     */
    static auto
    GenerateOutputWaveData(size_t num_samples, float frequency, float amplitude, float phase_shift)
        -> CUFFTVec<ElemType>;
};

template<>
auto FFTTestPair<float>::GenerateInputWaveData(size_t num_samples,
                                               float frequency,
                                               float amplitude,
                                               float phase_shift) -> CUFFTVec<float>
{
    // Initialise out vector to have a real and an imaginary value for each sample. Will overwrite
    // the default 0 value for the real numbers, the imaginary ones will stay at 0
    CUFFTVec<float> input_data(num_samples * 2, 0);

    // Go through each sample
    for(size_t i = 0; i < num_samples; i++) {
        // Calculate the value we want for the real component of the ith sample
        float real = amplitude
                     * cosf(frequency * ((float)i / num_samples) * 2 * (float)M_PI + phase_shift);

        // Set the corresponding part of the array to have the calculated value
        input_data[i * 2] = real;
    }
    return input_data;
}

template<>
auto FFTTestPair<float>::GenerateOutputWaveData(size_t num_samples,
                                                float frequency,
                                                float amplitude,
                                                float phase_shift) -> CUFFTVec<float>
{
    // Initialise out vector to have a real and an imaginary value for each sample, each starting
    // with 0 an only overridden as needed.
    CUFFTVec<float> output_data(num_samples * 2, 0);

    // Calculate the real and imaginary components of the spike we expect to see.
    float real = amplitude * (float)(num_samples / 2) * cosf(phase_shift);
    float imag = amplitude * (float)(num_samples / 2) * sinf(phase_shift);

    // Assign them to the place in the array that we expect to see the spike
    output_data[2 * (int)std::abs(frequency)] = real;
    output_data[2 * (int)std::abs(frequency) + 1] = imag;

    // Hermetian symmetry means that we expect to see a similar spike, with the second one being
    // mirrored around the centre of the array
    output_data[2 * (num_samples - std::abs(frequency))] = real;
    output_data[2 * (num_samples - std::abs(frequency)) + 1] = imag;

    return output_data;
}

TEST(Re_Workers_CUDAWorker_FFT, ConstructorDestructor)
{
    DeviceList devices;
    std::unique_ptr<CUFFTHandle> handle;
    // Call constructor
    ASSERT_NO_THROW(handle = std::make_unique<CUFFTHandle>(devices));

    // Call destructor
    ASSERT_NO_THROW(handle.reset());
}

TEST(RE_Workers_CUDAWorker_FFT, BasicFFT)
{
    // Create a sine wave of frequency 5 Hz
    FFTTestPair<float> testPair(128, 5, 1, 0);
    CUFFTVec<float> in_vec = testPair.GetInput();
    CUFFTVec<float> expected_vec = testPair.GetExpectedOutput();

    // Initialise the FFT plan
    DeviceList devices;
    CUFFTHandle cufft(devices);

    // Perform the FFT
    CUFFTVec<float> out_vec;
    ASSERT_NO_THROW(out_vec = cufft.FFT(in_vec));

    // If the output vector isn't the expected size then we have some big problems and shouldn't go
    // any further
    ASSERT_EQ(expected_vec.size(), out_vec.size());
    for(size_t index = 0; index < out_vec.size(); index++) {
        EXPECT_NEAR(expected_vec.at(index), out_vec.at(index), 1.0E-3);
    }
}

} // namespace cditma::cuda::test