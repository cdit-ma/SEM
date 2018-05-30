#include "opencl_worker.h"
#include "openclutilities.h"
#include <clFFT.h>

bool OpenCL_Worker::InitFFT() {
	clfftStatus err;
    fftSetupData = new clfftSetupData();
	err = clfftInitSetupData(fftSetupData);
    if (err != clfftStatus::CLFFT_SUCCESS) {
        Log(std::string(__func__), ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
            "Unable to successfully initialise clFFT setup data");
        return false;
    }
    err = clfftSetup(fftSetupData);
    if (err != clfftStatus::CLFFT_SUCCESS) {
        Log(std::string(__func__), ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
            "Unable to successfully set up the clFFT library");
        return false;
    }
    return true;
}

bool OpenCL_Worker::CleanupFFT() {
	clfftStatus err;
	err = clfftTeardown();
    if (err != clfftStatus::CLFFT_SUCCESS) {
		Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), "Could not successfully tear down clFFT library");
        return false;
    }
    return true;
};


bool OpenCL_Worker::FFT(std::vector<float> &data) {

	auto work_id = get_new_work_id();
    if (!is_valid_) {
		Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, work_id, "Unable to perform FFT calculations, worker is invalid!");
		return false;
	}

	cl_int err;

	// Find the size of each datapoint and how many datapoints there are
	//size_t pointSize = sizeof(float);
	//size_t N = dataBytes/pointSize;
    size_t N = data.size()/2;

	// Check that prime decomposition only uses 2, 3 and 5 as clFFT doesnt support other prime numbers
	if (N==0) {
		Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, work_id, "Unable to perform FFT: Passed an array of length 0");
		return false;
	}
	while (N%2 == 0) N = N/2;
	while (N%3 == 0) N = N/3;
	while (N%5 == 0) N = N/5;
	if (N > 1) {
		Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, work_id, "Unable to perform FFT: Number of samples must be a mix of powers of 2, 3 and 5");
		return false;
	}
	N = data.size();

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

    unsigned int allocated_dev_index = load_balancer_->RequestDevice();

    OpenCLDevice& dev = manager_->GetDevices(*this)[allocated_dev_index];

    //for (auto& dev_ref : devices_) {
        //auto& dev = dev_ref.get();
        //auto& dev_queue = dev.GetQueue().GetRef()();
        cl_command_queue dev_queue = dev.GetQueue().GetRef()();
        /* Bake the plan. */
        err = clfftBakePlan(planHandle, 1, &dev_queue, NULL, NULL);

        /* Prepare OpenCL memory objects and place data inside them. */
        OCLBuffer<float>* buffer = manager_->CreateBuffer(*this, data, dev, true);
        //cl::Buffer bufX(*context, CL_MEM_READ_WRITE, N * pointSize);
        //err = queues[gpuNum]->enqueueWriteBuffer(bufX, CL_TRUE, 0, dataBytes, dataIn);

        /* Execute the plan. */
        //err = clfftEnqueueTransform(planHandle, CLFFT_FORWARD, 1, &(*(queues[gpuNum]))(), 0, NULL, NULL, &bufX(), NULL, NULL);
        err = clfftEnqueueTransform(planHandle, CLFFT_FORWARD, 1, &dev_queue, 0, NULL, NULL, &(buffer->GetBackingRef()()), NULL, NULL);

        if (err != CL_SUCCESS) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
                "Failed to enqueue FFT transform with error message: "+OpenCLErrorName(err));
        }

        /* Wait for calculations to be finished. */
        err = dev.GetQueue().GetRef().finish();

        if (err != CL_SUCCESS) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
                "An error occurred while waiting for enqueued FFT transform to finish: "+OpenCLErrorName(err));
        }

        /* Fetch results of calculations. */
        //dev.GetQueue().enqueueReadBuffer(bufX, CL_TRUE, 0, dataBytes, dataIn);
        data = buffer->ReadData(*this, dev, true);

        /* Release the plan. */
        err = clfftDestroyPlan( &planHandle );
    //}

    load_balancer_->ReleaseDevice(allocated_dev_index);

    return true;
}