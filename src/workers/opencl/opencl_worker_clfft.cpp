#include "opencl_worker.h"
#include "openclutilities.h"
#include <clFFT.h>

bool OpenCL_Worker::InitFFT(int work_id) {
	clfftStatus err;
    fftSetupData = new clfftSetupData();
	err = clfftInitSetupData(fftSetupData);
    if (err != clfftStatus::CLFFT_SUCCESS) {
        Log(std::string(GET_FUNC), Logger::WorkloadEvent::ERROR, work_id,
            "Unable to successfully initialise clFFT setup data");
        return false;
    }
    err = clfftSetup(fftSetupData);
    delete fftSetupData;
    if (err != clfftStatus::CLFFT_SUCCESS) {
        Log(std::string(GET_FUNC), Logger::WorkloadEvent::ERROR, work_id,
            "Unable to successfully set up the clFFT library");
        return false;
    }
    return true;
}

bool OpenCL_Worker::CleanupFFT(int work_id) {
	clfftStatus err;
	err = clfftTeardown();
    if (err != clfftStatus::CLFFT_SUCCESS) {
		Log(GET_FUNC, Logger::WorkloadEvent::ERROR, work_id, "Could not successfully tear down clFFT library");
        return false;
    }
    return true;
};


bool OpenCL_Worker::FFT(std::vector<float> &data) {

	auto work_id = get_new_work_id();

    Log(GET_FUNC, Logger::WorkloadEvent::STARTED, work_id, "Beginning FFT with vector data");

    if (!is_valid_) {
		Log(__func__, Logger::WorkloadEvent::ERROR, work_id, "Unable to perform FFT calculations, worker is invalid");
		return false;
	}

    unsigned int allocated_dev_index = load_balancer_->RequestDevice();

    auto& dev = manager_->GetDevices(*this)[allocated_dev_index];

    /* Prepare OpenCL memory objects and place data inside them. */
    OpenCLBuffer<float> buffer = manager_->CreateBuffer(*this, data, *dev, true);

    auto success = FFT(buffer, allocated_dev_index, work_id);

    /* Fetch results of calculations. */
    data = buffer.ReadData(*this, *dev, true);

    load_balancer_->ReleaseDevice(allocated_dev_index);

    Log(GET_FUNC, Logger::WorkloadEvent::FINISHED, work_id);

    return success;
}


bool OpenCL_Worker::FFT(OpenCLBuffer<float>& buffer, int device_id, int work_id) {

    // If we haven't already been given a work_id by a calling function then generate one
    bool is_base_worker_call = false;
    if (work_id == -1) {
        is_base_worker_call = true;
        Log(GET_FUNC, Logger::WorkloadEvent::STARTED, work_id, "Beginning FFT with buffered data");
        work_id = get_new_work_id();
    }

    if (!is_valid_) {
		Log(__func__, Logger::WorkloadEvent::MESSAGE, work_id, "Unable to perform FFT calculations, worker is invalid");
		return false;
	}

    bool did_request_device = false;
    if(device_id == -1){
        //If we haven't got a valid device_id, get one from the load_balancer
        device_id = load_balancer_->RequestDevice();
        did_request_device = true;
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
        Log(__func__, Logger::WorkloadEvent::ERROR, get_new_work_id(),
            "Failed to enqueue FFT transform with error message: "+OpenCLErrorName(err));
    }

    /* Wait for calculations to be finished. */
    err = dev->GetQueue().GetRef().finish();

    if (err != CL_SUCCESS) {
        Log(__func__, Logger::WorkloadEvent::ERROR, get_new_work_id(),
            "An error occurred while waiting for enqueued FFT transform to finish: "+OpenCLErrorName(err));
    }

    /* Release the plan. */
    err = clfftDestroyPlan( &planHandle );

    if(did_request_device){
        load_balancer_->ReleaseDevice(device_id);
    }

    if (is_base_worker_call) {
        Log(GET_FUNC, Logger::WorkloadEvent::FINISHED, work_id);
    }

    return true;
}