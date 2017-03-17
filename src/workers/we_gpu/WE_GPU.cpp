// WE_GPU.mpc  2015-08-17  Jackson Michael 

#include "WE_GPU_Impl.h" 
#include "WE_GPU.h"
#include "atmsp.h"
#include <iostream>
#include <sstream>

/**
 * The WE_GPU class acts is a wrapper for WE_GPU_Impl, allowing projects to be compiled
 * without having to link against the dependencies requied by the proper GPU worker
 * implementation.
 */

WE_GPU::WE_GPU (void) : impl_(new WE_GPU_Impl ()){
	Initialise(false);
}

WE_GPU::~WE_GPU (void) {
	delete impl_;
}

void WE_GPU::Release(){
	impl_->Release();
}

void WE_GPU::Initialise(bool forceGPU){
	impl_->Initialise(forceGPU);
}

unsigned int WE_GPU::NumDevices(){
	return impl_->NumDevices();
}

std::string WE_GPU::DeviceName(unsigned int gpuNum){
	return impl_->DeviceName();
}

size_t WE_GPU::MemCapacity(unsigned int gpuNum){
	return impl_->MemCapacity();
}

bool WE_GPU::BufferData(size_t bytes, bool forcedCopy, bool blocking, unsigned int gpuNum){
	return impl_->BufferData(bytes, forcedCopy, blocking, gpuNum);
}

bool WE_GPU::ReleaseData(size_t bytes, bool forceCopy, bool blocking, unsigned int gpuNum){
	return impl_->ReleaseData(bytes, forceCopy, blocking, gpuNum);
}

void WE_GPU::RunParallel(double numThreads, double opsPerThread, unsigned int gpuNum){

	this->impl_->RunParallel((unsigned int)numThreads, (unsigned int)opsPerThread, gpuNum);
}

void WE_GPU::FFT(std::vector<float> &data, unsigned int gpuNum){
	this->impl_->PerformFFT_SP(data.data(), data.size()*sizeof(float), gpuNum);
}

void WE_GPU::MatrixMultLazy(unsigned int n, unsigned int gpuNum){
	this->impl_->MatrixMult(n, gpuNum);
}

bool WE_GPU::MatrixMult(const std::vector<float> &matrixA, const std::vector<float> &matrixB,
						std::vector<float> &matrixC, unsigned int gpuNum){
	return this->impl_->MatrixMult(matrixA.size(), matrixB.size(), matrixC.size(),
								   matrixA.data(), matrixB.data(), matrixC.data(), gpuNum);
}
