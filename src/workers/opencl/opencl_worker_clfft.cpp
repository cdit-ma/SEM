#include "opencl_worker.h"
#include "openclutilities.h"
#include <clFFT.h>

bool OpenCL_Worker::InitFFT() {
	clfftStatus err;
    fftSetupData = new clfftSetupData();
	err = clfftInitSetupData(fftSetupData);
    if (err != clfftStatus::CLFFT_SUCCESS) {
        Log(std::string(GET_FUNC), Logger::WorkloadEvent::ERROR, get_new_work_id(),
            "Unable to successfully initialise clFFT setup data");
        return false;
    }
    err = clfftSetup(fftSetupData);
    delete fftSetupData;
    if (err != clfftStatus::CLFFT_SUCCESS) {
        Log(std::string(GET_FUNC), Logger::WorkloadEvent::ERROR, get_new_work_id(),
            "Unable to successfully set up the clFFT library");
        return false;
    }
    return true;
}

bool OpenCL_Worker::CleanupFFT() {
	clfftStatus err;
	err = clfftTeardown();
    if (err != clfftStatus::CLFFT_SUCCESS) {
		Log(GET_FUNC, Logger::WorkloadEvent::ERROR, get_new_work_id(), "Could not successfully tear down clFFT library");
        return false;
    }
    return true;
};


bool OpenCL_Worker::FFT(std::vector<float> &data) {

    if (!is_valid_) {
		Log(__func__, Logger::WorkloadEvent::MESSAGE, get_new_work_id(), "Unable to perform FFT calculations, worker is invalid");
		return false;
	}

    unsigned int allocated_dev_index = load_balancer_->RequestDevice();

    auto& dev = manager_->GetDevices(*this)[allocated_dev_index];

    /* Prepare OpenCL memory objects and place data inside them. */
    OpenCLBuffer<float> buffer = manager_->CreateBuffer(*this, data, *dev, true);

    auto success = FFT(buffer, allocated_dev_index);

    /* Fetch results of calculations. */
    data = buffer.ReadData(*this, *dev, true);

    load_balancer_->ReleaseDevice(allocated_dev_index);

    return success;
}


bool OpenCL_Worker::FFT(OpenCLBuffer<float>& buffer, int device_id) {

	auto work_id = get_new_work_id();
    if (!is_valid_) {
		Log(__func__, Logger::WorkloadEvent::MESSAGE, work_id, "Unable to perform FFT calculations, worker is invalid");
		return false;
	}

    auto& dev = manager_->GetDevices(*this)[device_id];

	cl_int err;
    
    size_t length = buffer.GetNumElements();

    if(length % 2 != 0){
		Log(GET_FUNC, Logger::WorkloadEvent::ERROR, work_id, "Unable to perform FFT: Non-even length of array, implies half a sample (No complex number)");
        return false;
    }

	// Find the size of each datapoint and how many datapoints there are
    size_t N = length / 2;

	// Check that prime decomposition only uses 2, 3 and 5 as clFFT doesnt support other prime numbers
	if (N==0) {
		Log(GET_FUNC, Logger::WorkloadEvent::ERROR, work_id, "Unable to perform FFT: Passed an array of length 0");
		return false;
	}
	while (N%2 == 0) N = N/2;
	while (N%3 == 0) N = N/3;
	while (N%5 == 0) N = N/5;
	if (N > 1) {
		Log(GET_FUNC, Logger::WorkloadEvent::ERROR, work_id, "Unable to perform FFT: Number of samples must be a mix of powers of 2, 3 and 5");
		return false;
	}
	N = length;

    /* FFT library realted declarations */
    clfftDim dim = CLFFT_1D;
	size_t clLengths[1] = {N};

    /* Create a default plan for a complex FFT. */
	clfftPlanHandle planHandle;

    err = clfftCreateDefaultPlan(&planHandle, manager_->GetContext()(), dim, clLengths);

    /* Set plan parameters. */
    err = clfftSetPlanPrecision(planHandle, CLFFT_SINGLE);
    //err = clfftSetLayout(planHandle, CLFFT_COMPLEX_INTERLEAVED, CLFFT_COMPLEX_INTERLEAVED);
    //err = clfftSetLayout(planHandle, CLFFT_COMPLEX_INTERLEAVED, CLFFT_COMPLEX_PLANAR);
	err = clfftSetLayout(planHandle, CLFFT_REAL, CLFFT_HERMITIAN_INTERLEAVED);
    err = clfftSetResultLocation(planHandle, CLFFT_INPLACE);

    cl_command_queue dev_queue = dev->GetQueue().GetRef()();
    /* Bake the plan. */
    err = clfftBakePlan(planHandle, 1, &dev_queue, NULL, NULL);

    /* Execute the plan. */
    err = clfftEnqueueTransform(planHandle, CLFFT_FORWARD, 1, &dev_queue, 0, NULL, NULL, &(buffer.GetBackingRef()()), NULL, NULL);

    if (err != CL_SUCCESS) {
        Log(__func__, Logger::WorkloadEvent::MESSAGE, get_new_work_id(),
            "Failed to enqueue FFT transform with error message: "+OpenCLErrorName(err));
    }

    /* Wait for calculations to be finished. */
    err = dev->GetQueue().GetRef().finish();

    if (err != CL_SUCCESS) {
        Log(__func__, Logger::WorkloadEvent::MESSAGE, get_new_work_id(),
            "An error occurred while waiting for enqueued FFT transform to finish: "+OpenCLErrorName(err));
    }

    /* Release the plan. */
    err = clfftDestroyPlan( &planHandle );

    return true;
}