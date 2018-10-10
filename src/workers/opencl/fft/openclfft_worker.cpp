#include "openclfft_worker.h"

#include "../openclutilities.h"

#include "../openclmanager.h"
#include "../openclloadbalancer.h"

#include <clFFT.h>

OpenCLFFTWorker::OpenCLFFTWorker(const Component& component, std::string inst_name)
    : Worker(component, __func__, inst_name) {
    
    platform_id_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::INTEGER, "platform_id").lock();
    if (platform_id_ != NULL) {
        platform_id_->set_Integer(-1);
    }

    device_id_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::INTEGER, "device_id").lock();
    if (device_id_ != NULL) {
        device_id_->set_Integer(-1);
    }

    InitCLFFT();
}

OpenCLFFTWorker::~OpenCLFFTWorker() {
    CleanupCLFFT();
}

bool OpenCLFFTWorker::HandleConfigure() {
    int platform_id;
    int device_id;
    auto platform_attr = GetAttribute("platform_id").lock();
    if (platform_attr != NULL) {
        platform_id = platform_attr->get_Integer();
    }
    auto device_attr = GetAttribute("device_id").lock();
    if (device_attr != NULL) {
        device_id = device_attr->get_Integer();
    }

    manager_ = OpenCLManager::GetReferenceByPlatformID(*this, platform_id);
    if (manager_ == NULL) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
            "Unable to obtain a reference to an OpenCLManager");
        return false;
    }

    auto& device_list = manager_->GetDevices(*this);
    std::vector<unsigned int> device_ids;

    if (device_id < -1 || device_id >= device_list.size()) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
            "'device_id' provided by attribute is out of bounds: " + std::to_string(device_id));
        return false;
    }

    if (device_id == -1) {
        for (int i=0; i<device_list.size(); i++) {
            devices_.emplace_back(device_list.at(i));
            device_ids.push_back(i);
        }
    } else {
        devices_.emplace_back(device_list.at(std::ref(device_id)));
        device_ids.push_back(device_id);
    }

    load_balancer_ = new OpenCLLoadBalancer(device_ids);

    is_valid_ = true;
    return true;
}

void OpenCLFFTWorker::InitCLFFT() {
	clfftStatus err;
    fftSetupData = new clfftSetupData();
	err = clfftInitSetupData(fftSetupData);
    err = clfftSetup(fftSetupData);
}

void OpenCLFFTWorker::CleanupCLFFT() {
	clfftStatus err;
	err = clfftTeardown();
}


bool OpenCLFFTWorker::FFT(std::vector<float> &data) {
	auto work_id = get_new_work_id();
    if (!is_valid_) {
		Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, work_id, "Unable to perform FFT calculations, worker is invalid!");
		return false;
	}

	cl_int err;

	// Find the size of each datapoint and how many datapoints there are
	size_t pointSize = sizeof(float);
	//size_t N = dataBytes/pointSize;
    size_t N = data.size();

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
	err = clfftSetLayout(planHandle, CLFFT_REAL, CLFFT_HERMITIAN_INTERLEAVED);
    err = clfftSetResultLocation(planHandle, CLFFT_INPLACE);

    for (auto& dev_ref : devices_) {
        auto& dev = dev_ref.get();
        auto& dev_queue = dev.GetQueue().GetRef()();
        /* Bake the plan. */
        err = clfftBakePlan(planHandle, 1, &dev_queue, NULL, NULL);

        /* Prepare OpenCL memory objects and place data inside them. */
        OpenCLBuffer<float>* buffer = manager_->CreateBuffer(*this, data, dev, true);
        //cl::Buffer bufX(*context, CL_MEM_READ_WRITE, N * pointSize);
        //err = queues[gpuNum]->enqueueWriteBuffer(bufX, CL_TRUE, 0, dataBytes, dataIn);

        /* Execute the plan. */
        //err = clfftEnqueueTransform(planHandle, CLFFT_FORWARD, 1, &(*(queues[gpuNum]))(), 0, NULL, NULL, &bufX(), NULL, NULL);
        err = clfftEnqueueTransform(planHandle, CLFFT_FORWARD, 1, &dev_queue, 0, NULL, NULL, &(buffer->GetBackingRef()()), NULL, NULL);

        /* Wait for calculations to be finished. */
        dev.GetQueue().GetRef().finish();

        /* Fetch results of calculations. */
        //dev.GetQueue().enqueueReadBuffer(bufX, CL_TRUE, 0, dataBytes, dataIn);
        data = buffer->ReadData(*this, dev, true);

        /* Release the plan. */
        err = clfftDestroyPlan( &planHandle );
    }

    return true;
}

void OpenCLFFTWorker::Log(std::string function_name, ModelLogger::WorkloadEvent event, int work_id, std::string args) {
    Worker::Log("OpenCLFFTWorker::"+function_name, event, work_id, args);
    std::cerr << "OpenCLFFTWorker::" << function_name << ", " << args << std::endl;
}