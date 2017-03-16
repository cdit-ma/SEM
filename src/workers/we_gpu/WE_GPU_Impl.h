// WE_GPU.mpc  2015-08-17  Jackson Michael

#ifndef _WE_GPU_IMPL_H_
#define _WE_GPU_IMPL_H_

#include "cl.hpp"
#include "BufferHashtable.h"
#include <string>
#include <clFFT.h>

class WE_GPU_Impl {
public:
	WE_GPU_Impl();
	~WE_GPU_Impl();

	// Set up the OpenCL environment as well as creating kernels and buffers neccessary for
	// running kernels of arbitrary workitem sizes at a later point
	void initialise(bool forceGPU=true);
	
	// Clean up all openCL related data, including any manually allocated buffers that havent
	// been explicitly deallocated
	void release();

	// Returns whether or not the worker can currently perform meaningful GPU work
	bool isValid();

	// Just lets you know its alive
	void sayGreeting ();
	
	// Returns the name the device reports to openCL
	unsigned int numDevices();

	// Returns the name the device reports to openCL, or "INVALID WORKER"/"INVALID DEVICE" 
	std::string deviceName(unsigned int gpuNum=0);

	// Returns the amount of memory available in bytes for the indicated device
	size_t memCapacity(unsigned int gpuNum=0);

	// Buffer a given amount of data (in kilobytes), optionally copying that across to the GPU by default
	bool bufferData(size_t bytes, bool forceCopy=true, bool blocking=false, unsigned int gpuNum=0);

	// Release a given amount of buffers
	bool releaseData(size_t bytes, bool forceCopy, bool blocking=true, unsigned int gpuNum=0);

	// Run a highly parallel task using pre-buffered data
	void runParallel(unsigned int numThreads, unsigned int opsPerThread, unsigned int gpuNum=0);

	// Run several FFTs using totalBytes of input data, with each FFT processing sampleLength of data.
	// sampleLength should be a power of 2, and will be converted 
	void performFFT_SP(void* dataIn, size_t dataBytes, unsigned int gpuNum=0);
	void performFFT_SP(size_t dataBytes, unsigned int gpuNum=0);

	// Multiply two matrices of size n x n
	void matrixMult(unsigned int n, unsigned int gpuNum=0);
	// Multiply two matrices using memory from dataA and dataB
	// with the product placed into dataC, returns whether or not operation suceeded
	bool matrixMult(unsigned int widthA, unsigned int heightA, unsigned int widthB,
					void* dataA, void* dataB, void* dataOut,
					int gpuNum=0);


private:
	bool valid;

	// OpenCL data structures
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;

	cl::Context* context;
	std::vector<cl::CommandQueue*> queues;
	cl::Program* program;
	cl::Kernel* parallelKernel;
	cl::Kernel* partiallyParallelKernel;
	cl::Kernel* matrixKernel;

	//ACE_Thread_Mutex matrixLock_;

	// Blank buffers for allowing kernels to work on data
	cl::Buffer* blankBuffer;
	cl::Buffer* blankBuffer2;
	cl::Buffer* blankBuffer3;

	cl_uint blankdatasize;
	cl_uint numCentroids;

	// Hashtable for recording which buffers have been allocated
	BufferHashtable* hashtable;

	// Things should not be sharing this!!!!!
	// Make a list that deletes the relevant entry when the event's callback is triggered instead!
	cl::Event readCopyFinished;

	// FFT plan handle
    clfftSetupData fftSetup;

	void initCLFFT();
	void cleanupCLFFT();
};

#endif