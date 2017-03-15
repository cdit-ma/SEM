// WE_GPU.mpc  2015-08-17  Jackson Michael 

#ifndef _WE_GPU_H_
#define _WE_GPU_H_

#include <string>

// Requires vector wrapper from the utility worker
#include "../we_ute/WE_UTE.h"

class WE_GPU_Impl;

/** \brief A CUTS worker that provides GPGPU functionality 
 * 
 * Two methods of modelling GPGPU workloads are provided; resource-usage and drop-in-workload.
 * 
 * The drop-in-workload functions seek to provide a simple interface for common GPGPU activities,
 * providing functionality while requiring minimal effort and GPGPU knowledge on the part of the
 * user. As the specific nature of the computation to be carried out is not known before hand there
 * may be some modelling artifacts, such as run time compilation happening just-in-time, where it
 * might usually happen during initialisation.
 * 
 * Resource usage provides functionality such as allocating buffer visible to GPUs, copying data
 * into them, using those buffers to execute kernels... essentially any of the "high level"
 * operations that must be considered when writing CUDA/OpenCL code. As this worker is intended
 * to simplify GPGPU operations it doesn't offer the full flexibility of a GPGPU language. If 
 * this is desired then it could be achieved by either extending this worker or creating an
 * alternate one to provide the specific functionality reqiured.
 */
class WE_GPU { 
public:
	/**
	 * Creates a GPU worker and initialises it so that it will consider all OpenCL devices
	 * as valid targets.
	 * @TODO provide a method for preventing the worker from being stuck using only the CPU
	 *       due to being locked to one platform (eg grabs intel plaform with no Xeon Phi).
	 */
	WE_GPU (void);

	/**
	 * Delete the GPU worker and release any of its GPU related resources.
	 */
	~WE_GPU(void);

	/**
	 * Initialise the GPU worker so that it is valid for further use, optionally specifying whether or not
	 * it should consider using the CPU as an OpenCL device.
	 *
	 * @param forceGPU Indicates whether or not the worker should ignore any OpenCL capable devices that arent GPUs
	 */
	void initialise(bool forceGPU=true);

	/**
	 * Release any GPU related resources.
	 */
	void release();

	/**
	 * Quick print-test to stdout for whether or not the worker is alive
	 */
	void sayGreeting();
	
	/**
	 * Returns the number of devices the worker is currently aware of
	 */
	unsigned int numDevices();

	/**
	 * Get the device name reported to OpenCL by a given device

	 * @param gpuNum The index of the target GPU/device (starting from 0)
	 * 
	 * @return The name of the device, or "INVALID WORKER"/"INVALID DEVICE"
	 */
	std::string deviceName(unsigned int gpuNum=0);

	/**
	 * Get the amount of global memory that a device reports itself as having to OpenCL.
	 * Note that this is not the same as the amount of available memory!
	 *
	 * @param gpuNum The index of the target GPU/device (starting from 0)
	 * 
	 * @return The amount of memory in bytes, or 0 if the size couldn't be determined
	 */
	size_t memCapacity(unsigned int gpuNum=0);
  
	/**
	 * Buffer a given amount of data, optionally copying that across to the GPU by default
	 *
	 * @param bytes The amount of bytes that the buffer should be able to hold
	 * @param forceCopy Whether or not the specified amount of data should be immediately copied to the GPU
	 * @param blocking Whether to pause until the copy operation has finished (if a copy occurs)
	 * @param gpuNum The index of the target GPU/device (starting from 0)
	 * 
	 * @return Whether or not the allocation (and copy if specified) successfully completed
	 */
	bool bufferData(size_t bytes, bool forceCopy=true, bool blocking=false, unsigned int gpuNum=0);

	/**
	 * Deallocate a buffer of a given size, if one exists
	 * 
	 * @param bytes The number of bytes in the buffer to be deallocated
	 * @param forceCopy Whether or not to read the buffer data back from a GPU before deleting
	 * @param blocking Whether to pause until the read is complete
	 * @param gpuNum The index of the target GPU/device (starting from 0)
	 *
	 * @return Whether or not the deallocation (and read if specified) successfully completed
	 */
	bool releaseData(size_t bytes, bool forceCopy=false, bool blocking=true, unsigned int gpuNum=0);

	/**
	 * Launch a kernel to run on a GPU, executing as many of the specified number of threads
	 * as is possible at once. The amountof work each thread does will scale linearly with the
	 * specified amount of operations per thread. Doesn't contain factors such as excessive
	 * global memory reads or inter-work-item synchronisation that could result in performance
	 * scaling better than some kernels as the number of work-items increases.
	 *
	 * @param workItems The amount of work-items to be spawned.
	 * @param opsPerThread The amount of work that should be done by each thread
	 * @param gpuNum The index of the target GPU/device (starting from 0)
	 */
	void runParallel(double numThreads, double opsPerThread, unsigned int gpuNum=0);

	/**
	 * Run an FFT usign the given data, with the result overwriting the input array. The input
	 * should be an array of real floats, which is replaced by the an array of interleaved real
	 * and imaginary numbers stored as floats, with the complex conjugates discarded (Hermetian).
	 * Note: as each FFT in O(n*log(n)) wrt sampleLength execution times should increase as sampleLength
	 * is increased. A decrease is potentially a sign that initialisation and memory transfer bottlenecks
	 * are overwhelming compute bottlenecks.
	 *
	 * @param data An array of floats whose length is combination of powers of 2, 3 and 5
	 * @param gpuNum The index of the target GPU/device (starting from 0)
	 */
	//void FFT(WE_UTE_Vector data, unsigned int gpuNum=0);
	//void FFT_singlePrecision(size_t totalBytes, unsigned int gpuNum=0);
	//template<CORBA::ULong S>
	//void FFT(TAO::bounded_value_sequence<CORBA::Float, S> data, unsigned int gpuNum=0);

	/**
	 * Multiply matrix A (of size NxK) by B (of size KxM) to produce C (of size NxM).
	 * The dimensions of each matrix will be inferred from vectors passed in.
	 * 
	 * @param matrixA The first of the two matrix operands
	 * @param matrixB The second of the two matrix operands
	 * @param matrixC The matrix to be filled with the result of the multiplication.
	 * @param gpuNum The index of the target GPU/device (starting from 0)
	 *
	 * @result Simply returns a shallow copy of the vector provided for matrixC (used to assist
	 *         MEDEAs representation).
	 */
	// WE_UTE_Vector matrixMult(WE_UTE_Vector matrixA,
	// 						WE_UTE_Vector matrixB,
	// 						WE_UTE_Vector matrixC,
	// 						unsigned int gpuNum=0);
	//template<class T, CORBA::ULong S>
	//template<CORBA::ULong S, CORBA::ULong T, CORBA::ULong U>
	//void matrixMult(TAO::bounded_value_sequence<CORBA::Float, S> matrixA,
	//				TAO::bounded_value_sequence<CORBA::Float, T> matrixB,
	//				TAO::bounded_value_sequence<CORBA::Float, U> matrixC,
	//				unsigned int gpuNum=0);

	/**
	 * Lightweight matrix multiplaction that doesn't require user to manually specify matrices,
	 * just goes ahead and multiplies two arbitrary NxN matrices to produce a third
	 * 
	 * @param n The number of rows and number of columns of each matrix
	 */
	void matrixMultLazy(unsigned int n, unsigned int gpuNum=0);

private:
	/**
	 * A reference to the class containing the actual worker implementation.
	 */
	WE_GPU_Impl * impl_;
	
	// Called by a variadic function that has been given a complexity and an arbitrary number of
	// parameters, evaluating the expression. All parameters should be passed in as doubles/floats.
	//double evalComplexity(std::string complexity, va_list* argList);

	/*
	template<class T, CORBA::ULong S>
	size_t getByteSize(TAO::bounded_value_sequence<T, S> seq);

	template<class T>
	size_t getByteSize(T obj);

	template<class T, CORBA::ULong S>
	void* getAllocedMem(TAO::bounded_value_sequence<T, S> seq);
	*/
}; 




// Template implementations
/*template<class T, CORBA::ULong S>
void WE_GPU::FFT_singlePrecision(TAO::bounded_value_sequence<T, S> data, unsigned int gpuNum) {
	switch (getByteSize(T)) {
	case 4:
		this->impl_->performFFT_SP(getAllocedMem(data), gpuNum);
		break;
	default:
		BOOST_STATIC_ASSERT(sizeof(T)==0);	// Get out if unexpected size
	}
}
template<CORBA::ULong S>
void WE_GPU::FFT(TAO::bounded_value_sequence<CORBA::Float, S> data, unsigned int gpuNum) {
	this->impl_->performFFT_SP(data.get_buffer(), S*sizeof(float), gpuNum);
}

//template<class T, CORBA::ULong S>
template<CORBA::ULong S, CORBA::ULong T, CORBA::ULong U>
void WE_GPU::matrixMult(TAO::bounded_value_sequence<CORBA::Float, S> matrixA,
					TAO::bounded_value_sequence<CORBA::Float, T> matrixB, 
					TAO::bounded_value_sequence<CORBA::Float, U> matrixC,
					unsigned int gpuNum) {

	this->impl_->matrixMult(S, T, U,
							matrixA.get_buffer(), matrixB.get_buffer(), matrixC.get_buffer(),
							gpuNum);
}

template<class T, CORBA::ULong S>
size_t WE_GPU::getByteSize(TAO::bounded_value_sequence<T, S> seq) {
	return (getByteSize(seq[0]) * S);
}

template<class T>
size_t WE_GPU::getByteSize(T obj) {
	return sizeof(obj);
}

template<class T, CORBA::ULong S>
void* WE_GPU::getAllocedMem(TAO::bounded_value_sequence<T, S> seq) {
	return seq.get_buffer();
}
*/
#endif