// WE_GPU.mpc  2015-08-17  Jackson Michael 

#include "WE_GPU_Impl.h"
#include "clErrorPrinter.h"
#include <iostream>
#include <cmath>

// Returns with false if an error is encountered
#define clPrintErrorAndReturn(err, str) clPrintError(err, str); if (err != CL_SUCCESS)return false;

using namespace std;

// Load kernel code from openCL files
#define CL_KERNEL_CODE(code) #code
string clKernels = 
	#include "kernels.cl"
	;


// Callback for when a buffer has completed copying across some arbitrarily allocated memory
// that must be freed once the operation has finished
void CL_CALLBACK postReadCopyTeardown(cl_event event, cl_int blah, void* memory) {
	delete[] (char*)memory;
}


WE_GPU_Impl::WE_GPU_Impl() {
	// GPUworker isn't valid until it has been properly initialised
	valid = false;


	blankdatasize = 4096;
	numCentroids = 4;

	blankBuffer = NULL;
	blankBuffer2 = NULL;
	blankBuffer3 = NULL;

	hashtable = NULL;

	parallelKernel = NULL;
	partiallyParallelKernel = NULL;

	program = NULL;

	context = NULL;

	cout << "GPU worker created" << endl;
}

WE_GPU_Impl::~WE_GPU_Impl() {
	release();
}


// Set up the OpenCL environment so that subsequent calls have minimal overhead when executing workload
void WE_GPU_Impl::initialise(bool forceGPU) {
	if (valid) {
		cout << "Attempting to reinitialise an already valid GPU worker" << endl;
		return;
	}

	cl_int error;

	cout << "Initialising openCL as part of GPUWorker" << endl;

	// Get the list of all avaialble platforms
	error = cl::Platform::get(&platforms);
	if (error != CL_SUCCESS) {
		valid = false;
		cerr << "Couldn't find OpenCL platforms, OpenCL error code: " << clGetErrorName(error) << endl;
		return;
	}

	// Make sure that there is at least one implementation available
	if (platforms.size() > 1) {
		string platformName;
		platforms[0].getInfo(CL_PLATFORM_NAME, &platformName);
		cout << "NOTE: found more than one platform for OpenCL, using first found: " << platformName << endl;
	}

	cl_device_type devType;
	if (forceGPU) {
		devType = CL_DEVICE_TYPE_GPU;
	} else {
		devType = CL_DEVICE_TYPE_ALL;
	}

	// Find the first platform containing usable devices and store the associated device list
	for (unsigned int i=0; i<platforms.size(); i++) {
		string platformName;
		platforms[i].getInfo(CL_PLATFORM_NAME, &platformName);
		error = platforms[i].getDevices(devType, &devices);

		if (error != CL_SUCCESS) {
			cerr << "Couldn't find any devices for an openCL platform (" << platformName << "), OpenCL error code: " << clGetErrorName(error) << endl;
			continue;
		} else {
			// Display the list of devices found to the user
			cout << "Found devices for an openCL platform (" << platformName << "):" << endl;
			for (unsigned int j=0; j< devices.size(); j++) {
				string deviceName;
				devices[j].getInfo(CL_DEVICE_NAME, &deviceName);
				cout << "    " << j << ": " << deviceName << endl;
			}

			// If there are other platofrms but we've already found devices in another, notify the user and skip
			if (i < platforms.size()-1) {
				cout << "NOTE: More OpenCL platforms are present, skipping them as it is not possbile to have devices from different plaforms share a context" << endl;
			}
			break;
		}

	}
	
	// Ensure that one of the platform implementations contains at least one device that can be used
	if (devices.size() == 0) {
		valid = false;
		cout << "Couldn't find any openCL capable devices" << endl;
		return;
	}

	cout << "Found a device, starting to create contexts and programs" << endl;
	
	// Create an OpenCL context using the devices found
	context = new cl::Context(devices, NULL, NULL, NULL, &error);
	if (error != CL_SUCCESS) {
		valid = false;
		cerr << "Couldn't create OpenCL context: " << clGetErrorName(error) << endl;
		return;
	}

	// Create the command queues associated with each device
	for (unsigned int i=0; i<devices.size(); i++) {
		queues.push_back(new cl::CommandQueue(*context, devices[i]));
	}

	
	cl::Program::Sources sourceList(1, make_pair(clKernels.c_str(), clKernels.length()));
	program = new cl::Program(*context, sourceList, &error);
	if (error != CL_SUCCESS) {
		valid = false;
		cerr << "Couldn't create OpenCL program: " << clGetErrorName(error) << endl;
		return;
	}

	// Compile the code for every device in the selected platform
	error = program->build(devices);
	if (error != CL_SUCCESS) {
		string buildOutput;
		// On failure, print the build log for every device
		for (unsigned int devNum=0; devNum < devices.size(); devNum++) {
			program->getBuildInfo(devices[devNum], CL_PROGRAM_BUILD_LOG, &buildOutput);
			cout << buildOutput << endl;
		}
		valid = false;
		return;
	}

	// Create the kernels used for doing work from the now compiled program
	parallelKernel = new cl::Kernel(*program, "classifyPoints", &error);
	if (error != CL_SUCCESS) {
		valid = false;
		cerr << "Failed to create parallel kernel: " << clGetErrorName(error) << endl;
		return;
	}
	partiallyParallelKernel = new cl::Kernel(*program, "adjustCentroids", &error);
	if (error != CL_SUCCESS) {
		valid = false;
		cerr << "Failed to create partially parallel kernel: " << clGetErrorName(error) << endl;
		return;
	}
	matrixKernel = new cl::Kernel(*program, "matrixMultFull", &error);
	if (error != CL_SUCCESS) {
		valid = false;
		cerr << "Failed to create parallel kernel: " << clGetErrorName(error) << endl;
		return;
	}

	// Create empty buffers that allow kernels to do busy work
	blankBuffer = new cl::Buffer(*context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_WRITE, sizeof(cl_float4)*blankdatasize, NULL, &error);
	if (error != CL_SUCCESS) {
		valid = false;
		cerr << "Failed to create blank buffer 1: " << clGetErrorName(error) << endl;
		return;
	}
	blankBuffer2 = new cl::Buffer(*context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_WRITE, sizeof(cl_uint)*blankdatasize, NULL, &error);
	if (error != CL_SUCCESS) {
		valid = false;
		cerr << "Failed to create blank buffer 2: " << clGetErrorName(error) << endl;
		return;
	}
	blankBuffer3 = new cl::Buffer(*context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_WRITE, sizeof(cl_float4)*numCentroids, NULL, &error);
	if (error != CL_SUCCESS) {
		valid = false;
		cerr << "Failed to create blank buffer 3: " << clGetErrorName(error) << endl;
		return;
	}
	
	// Set the parellel kernel's arguments for all subsequent calls
	error = parallelKernel->setArg(0, *blankBuffer);
	if (error != CL_SUCCESS) {
		valid = false;
		cerr << "Failed to set parallel kernel argument 0: " << clGetErrorName(error) << endl;
		return;
	}
	error = parallelKernel->setArg(1, *blankBuffer2);
	if (error != CL_SUCCESS) {
		valid = false;
		cerr << "Failed to set parallel kernel argument 1: " << clGetErrorName(error) << endl;
		return;
	}
	error = parallelKernel->setArg(2, sizeof(cl_uint), &blankdatasize);
	if (error != CL_SUCCESS) {
		valid = false;
		cerr << "Failed to set parallel kernel argument 2: " << clGetErrorName(error) << endl;
		return;
	}
	error = parallelKernel->setArg(3, *blankBuffer3);
	if (error != CL_SUCCESS) {
		valid = false;
		cerr << "Failed to set parallel kernel argument 3: " << clGetErrorName(error) << endl;
		return;
	}
	error = parallelKernel->setArg(4, sizeof(cl_uint), &numCentroids);
	if (error != CL_SUCCESS) {
		valid = false;
		cerr << "Failed to set parallel kernel argument 4: " << clGetErrorName(error) << endl;
		return;
	}

	// Create the hashtable that will store all buffers that were allocated on user request
	hashtable = new BufferHashtable(context);

	// Perform required single-run setup for clFFT
	initCLFFT();

	// Mark that the GPUWorker is now good to go
	valid = true;
}

void WE_GPU_Impl::release() {
	// Currently avoids deletion if GPUWorker is not in entirely valid state,
	// should instead check validity of each OpenCL component and release accordingly
	if (!valid) {
		return;
	}

	valid = false;

	cleanupCLFFT();

	delete blankBuffer;
	delete blankBuffer2;
	delete blankBuffer3;

	delete hashtable;
	delete parallelKernel;
	delete partiallyParallelKernel;

	cout << "Tearing down OpenCL for GPUWorker" << endl;

	delete program;

	for (unsigned int i=0; i<queues.size(); i++) {
		delete queues[i];
	}
	delete context;
}

bool WE_GPU_Impl::isValid() {
	return valid;
}

void WE_GPU_Impl::sayGreeting () {
	if (!valid) {
		cerr << "Unable to say greeting, GPU worker is invalid!" << endl;
		return;
	}

  std::cout << "GPU WORKER SAYS HI" << std::endl;   
}

unsigned int WE_GPU_Impl::numDevices() {
	return devices.size();
}

string WE_GPU_Impl::deviceName(unsigned int gpuNum) {
	if (!valid) {
		cerr << "Unable to retrieve device name, GPU worker is invalid!" << endl;
		return "INVALID WORKER";
	}

	if (gpuNum >= devices.size()) {
		cerr << "Unable to retrieve device name, device number is larger than amount of devices" << endl;
		return "INVALID DEVICE";
	}

	string devName;
	devices[gpuNum].getInfo(CL_DEVICE_NAME, &devName);
	return devName;
}


size_t WE_GPU_Impl::memCapacity(unsigned int gpuNum) {
	if (!valid) {
		cerr << "Unable to retrieve device memory capacity, GPU worker is invalid!" << endl;
		return 0;
	}

	if (gpuNum >= devices.size()) {
		cerr << "Unable to retrieve device memory capacity, device number is larger than amount of devices" << endl;
		return 0;
	}

	// This will return the amount of memory available to the implementation, not the total available on the device.
	cl_ulong memSize;
	devices[gpuNum].getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &memSize);
	return memSize;
}

bool WE_GPU_Impl::bufferData(size_t bytes, bool forceCopy, bool blocking, unsigned int gpuNum) {
	if (!valid) {
		cerr << "Unable to buffer data, GPU worker is invalid!" << endl;
		return false;
	}
	
	// Allocate a buffer of the required size
	cl::Buffer* buf = hashtable->allocate(bytes);
	if (buf == NULL) {
		return false;
	}

	if (forceCopy) {
		char* tempMem = new char[bytes];

		// Everyone else seems to set the callback AFTER enqueueing, but it seems more logical to
		// specify it before-hand in case the thread is paused, which seems to work perfectly fine...
		// but best stick to the spec.
		cl_int error;
		error = queues[gpuNum]->enqueueWriteBuffer(*buf, CL_FALSE, 0, bytes, tempMem, NULL, &readCopyFinished);

		if (!blocking) {
			// Set a callback to delete the memory allocated once the write has completed
			error = readCopyFinished.setCallback(CL_COMPLETE, &postReadCopyTeardown, tempMem);
			if (error != CL_SUCCESS) {
				cerr << "Failed to register callback for buffer copy completion on allocation: " << clGetErrorName(error) << endl;
			}
		} else {
			delete[] tempMem;
		}
	}

	return true;
}

// Note: deleting a clBuffer has an underlying call to clReleaseMemObject, which promises to only delete
// after all queued commands related to an object are finished, making it safe for non-blocking usage
bool WE_GPU_Impl::releaseData(size_t bytes, bool forceCopy, bool blocking, unsigned int gpuNum) {
	if (!valid) {
		cerr << "Unable to release buffered data, GPU worker is invalid!" << endl;
		return false;
	}

	if (forceCopy) {
		// Search for a previously allocated buffer of the specified size
		cl::Buffer* buf = hashtable->get(bytes);
		if (buf == NULL) {
			cerr << "Unable to deallocate buffered data, couldn't find a buffer of size " << bytes << endl;
			return false;
		}

		char* tempMem = new char[bytes];

		// forceCopy has been set, so read back the buffer
		cl_int error;
		error = queues[gpuNum]->enqueueReadBuffer(*buf, blocking, 0, bytes, tempMem, NULL, &readCopyFinished);
		if (!blocking) {
			// readCopyFinished is not valid until set by the relevant enqueue command, so must be called second
			error = readCopyFinished.setCallback(CL_COMPLETE, &postReadCopyTeardown, tempMem);
			if (error != CL_SUCCESS) {
				cerr << "Failed to register callback for buffer copy completion on release: " << clGetErrorName(error) << endl;
			}
		} else {
			delete[] tempMem;
		}
	}

	// Deallocate the specified amount of data
	bool success = hashtable->deallocate(bytes);
	if (!success) {
		cerr << "Unable to deallocate buffered data, couldn't find a buffer of size " << bytes << endl;
		return false;
	}

	return true;
}

void WE_GPU_Impl::runParallel(unsigned int numThreads, unsigned int opsPerThread, unsigned int gpuNum) {
	if (!valid) {
		cerr << "Unable to begin parallel task, GPU worker is invalid!" << endl;
		return;
	}

	cl_int error;

	// Create a one-dimensional NDRange to specify the amount of work-items requested by the caller
	cl::NDRange globalPointRange(numThreads);

	// Set the amount of operations performed per thread
	error = parallelKernel->setArg(4, sizeof(cl_uint), &opsPerThread);
	if (error != CL_SUCCESS) {
		valid = false;
		cerr << "Failed to set the number of kernel operations: " << clGetErrorName(error) << endl;
		return;
	}
	numCentroids = opsPerThread;

	cl::Event kernelEvent;

	error = queues[gpuNum]->enqueueNDRangeKernel(*parallelKernel, cl::NullRange, globalPointRange, cl::NullRange, NULL, &kernelEvent);
	if (error != CL_SUCCESS) {
		cerr << "Error when enqueueing parallel kernel: " << clGetErrorName(error) << endl;
	}

	error = kernelEvent.wait();
	if (error != CL_SUCCESS) {
		cerr << "Error while waiting for parallel kernel to finish: " << clGetErrorName(error) << endl;
	}


}


void WE_GPU_Impl::initCLFFT() {
	clfftStatus err;
	err = clfftInitSetupData(&fftSetup);
    err = clfftSetup(&fftSetup);
}

void WE_GPU_Impl::cleanupCLFFT() {
	clfftStatus err;
	err = clfftTeardown();
}


void WE_GPU_Impl::performFFT_SP(size_t dataBytes, unsigned int gpuNum) {
	
    float *tempData = (float *)malloc(dataBytes);
	performFFT_SP(tempData, dataBytes, gpuNum);
	free(tempData);
}

void WE_GPU_Impl::performFFT_SP(void* dataIn, size_t dataBytes, unsigned int gpuNum) {
	if (!valid) {
		cerr << "Unable to perform FFT calculations, GPU worker is invalid!" << endl;
		return;
	}

	cl_int err;

	// Find the size of each datapoint and how many datapoints there are
	size_t pointSize = sizeof(float);
	size_t N = dataBytes/pointSize;

	// Check that prime decomposition only uses 2, 3 and 5 as clFFT doesnt support other prime numbers
	if (N==0) {
		cerr << "GPU WORKER: Unable to perform FFT: Passed an array of length 0" << endl;
		return;
	}
	while (N%2 == 0) N = N/2;
	while (N%3 == 0) N = N/3;
	while (N%5 == 0) N = N/5;
	if (N > 1) {
		cerr << "GPU WORKER: Unable to perform FFT: Number of samples must be a mix of powers of 2, 3 and 5" << endl;
		return;
	}
	N = dataBytes/pointSize;

    /* FFT library realted declarations */
    clfftDim dim = CLFFT_1D;
	size_t clLengths[1] = {N};

    /* Create a default plan for a complex FFT. */
	clfftPlanHandle planHandle;
    err = clfftCreateDefaultPlan(&planHandle, (*context)(), dim, clLengths);

    /* Set plan parameters. */
    err = clfftSetPlanPrecision(planHandle, CLFFT_SINGLE);
    //err = clfftSetLayout(planHandle, CLFFT_COMPLEX_INTERLEAVED, CLFFT_COMPLEX_INTERLEAVED);
	err = clfftSetLayout(planHandle, CLFFT_REAL, CLFFT_HERMITIAN_INTERLEAVED);
    err = clfftSetResultLocation(planHandle, CLFFT_INPLACE);

    /* Bake the plan. */
    err = clfftBakePlan(planHandle, 1, &(*(queues[gpuNum]))(), NULL, NULL);

	/* Prepare OpenCL memory objects and place data inside them. */
	cl::Buffer bufX(*context, CL_MEM_READ_WRITE, N * pointSize);

	err = queues[gpuNum]->enqueueWriteBuffer(bufX, CL_TRUE, 0, dataBytes, dataIn);

	/* Execute the plan. */
	err = clfftEnqueueTransform(planHandle, CLFFT_FORWARD, 1, &(*(queues[gpuNum]))(), 0, NULL, NULL, &bufX(), NULL, NULL);

	/* Wait for calculations to be finished. */
	queues[gpuNum]->finish();

	/* Fetch results of calculations. */
	queues[gpuNum]->enqueueReadBuffer(bufX, CL_TRUE, 0, dataBytes, dataIn);

    /* Release the plan. */
    err = clfftDestroyPlan( &planHandle );
	
}

void WE_GPU_Impl::matrixMult(unsigned int n, unsigned int gpuNum) {
	// Single NxN matrix on the host side
	unsigned int matSize = n*n;
	cl_float* matData = new cl_float[matSize];

	// Can pass matData as all 3 params because data has to be buffered to GPU,
	// so no possibility of overwriting input data with output data during calculation
	// going by current implementation.
	matrixMult(matSize, matSize, matSize, matData, matData, matData, gpuNum);

	delete[] matData;
}

bool WE_GPU_Impl::matrixMult(unsigned int lenA, unsigned int lenB, unsigned int lenC,
							 void* dataA, void* dataB, void* dataOut,
							 int gpuNum) {
	cl_int error;

	// Determine the dimensions of the matrices from the lengths of the data
	if (lenA == 0 || lenB == 0 || lenC == 0) {
		if (lenA + lenB + lenC == 0) {
			cout << "GPU Worker: warning: performing multiplication on empty matrices" << endl;
			return true;
		} else {
			cerr << "Error during matrix multiplication; sizes of matrices don't match, skipping calculation" << endl;
			return false;
		}
	}

	cl_ulong Ksquared = ((cl_ulong)lenA*(cl_ulong)lenB)/lenC;
	cl_uint K = (cl_uint)sqrt((/*long*/ double)Ksquared);
	cl_uint M = lenA/K;
	cl_uint N = lenB/K;
	if ((cl_ulong)K*K != Ksquared || (cl_ulong)M*K != lenA || (cl_ulong)N*K != lenB) {
		cerr << "Error during matrix multiplication; sizes of matrices don't match, skipping calculation" << endl;
		return false;
	}

	// Allocating buffers to hold the data provided by caller
	cl::Buffer matA(*context, CL_MEM_ALLOC_HOST_PTR, lenA*sizeof(cl_float), NULL, &error);
	clPrintErrorAndReturn(error,  "allocating space for matrix A");
	cl::Buffer matB(*context, CL_MEM_ALLOC_HOST_PTR, lenB*sizeof(cl_float), NULL, &error);
	clPrintErrorAndReturn(error,  "allocating space for matrix B");
	cl::Buffer matC(*context, CL_MEM_ALLOC_HOST_PTR, lenC*sizeof(cl_float), NULL, &error);
	clPrintErrorAndReturn(error,  "allocating space for matrix C");

	// Explicitly ransfer data in buffers to device
	std::vector<cl::Event> copyEvents(2);
	error = queues[gpuNum]->enqueueWriteBuffer(matA, true, 0, lenA*sizeof(cl_float), dataA, NULL, &(copyEvents[0]));
	clPrintErrorAndReturn(error,  "enqueueing matrix write for Matrix A");
	error = queues[gpuNum]->enqueueWriteBuffer(matB, true, 0, lenB*sizeof(cl_float), dataB, NULL, &(copyEvents[1]));
	clPrintErrorAndReturn(error,  "enqueueing matrix write for Matrix B");
	
	queues[gpuNum]->flush();

	//cout << "setting args" << endl;
	unsigned int blockLength = 1;

	// Make sure that enough threads are spawned that blocking can be done horizontally and vertically
	unsigned int globalWidth = 0;
	while (globalWidth < M || globalWidth < K) globalWidth += blockLength;
	unsigned int globalHeight = 0;
	while (globalHeight < N || globalHeight < K) globalHeight += blockLength;
	
	//this->matrixLock_.acquire();

	// Set the parameters for the ma trix multiplication
	error = matrixKernel->setArg(0, matA);
	clPrintErrorAndReturn(error, "setting argument 0 of matrix multiplication kernel");
	error = matrixKernel->setArg(1, matB);
	clPrintErrorAndReturn(error, "setting argument 1 of matrix multiplication kernel");
	error = matrixKernel->setArg(2, matC);
	clPrintErrorAndReturn(error, "setting argument 2 of matrix multiplication kernel");
	error = matrixKernel->setArg(3, M);
	clPrintErrorAndReturn(error, "setting argument 3 of matrix multiplication kernel");
	error = matrixKernel->setArg(4, K);
	clPrintErrorAndReturn(error, "setting argument 4 of matrix multiplication kernel");
	error = matrixKernel->setArg(5, N);
	clPrintErrorAndReturn(error, "setting argument 5 of matrix multiplication kernel");
	error = matrixKernel->setArg(6, cl::__local(blockLength*blockLength*sizeof(cl_float)));
	clPrintErrorAndReturn(error, "setting argument 6 of matrix multiplication kernel");
	error = matrixKernel->setArg(7, cl::__local(blockLength*blockLength*sizeof(cl_float)));
	clPrintErrorAndReturn(error, "setting argument 7 of matrix multiplication kernel");
	
	// Execute the kernel
	std::vector<cl::Event> executeEvents(1);
	error = queues[gpuNum]->enqueueNDRangeKernel(*matrixKernel, cl::NullRange, cl::NDRange(globalWidth, globalHeight), cl::NDRange(blockLength,blockLength), &copyEvents, &(executeEvents[0]));
	clPrintErrorAndReturn(error, "executing a multiplication kernel");

	//sthis->matrixLock_.release();

	// Forcibly flush and wait, this combo is kinda overkill but was found to have a better chance
	// of forcing the nvidia drivers to schedule properly
	queues[gpuNum]->flush();
	executeEvents[0].waitForEvents(executeEvents);

	// Read the result back
	error = queues[gpuNum]->enqueueReadBuffer(matC, true, 0, lenC*sizeof(cl_float), dataOut);
	clPrintErrorAndReturn(error, "Error when reading answer after matrix multiplication: ");

	return true;
}