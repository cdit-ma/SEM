// WE_GPU.mpc  2015-08-17  Jackson Michael 

#include "gpuworker_impl.h" 
#include "gpuworker.h"
#include <iostream>
#include <sstream>

/**
 * The WE_GPU class acts is a wrapper for WE_GPU_Impl, allowing projects to be compiled
 * without having to link against the dependencies requied by the proper GPU worker
 * implementation.
 */

GpuWorker::GpuWorker(Component* component, std::string inst_name) : Worker(component, __func__, inst_name){
    impl_ = new GpuWorker_Impl();
}

GpuWorker::~GpuWorker(void){
	delete impl_;
}

void GpuWorker::Release(){
	impl_->Release();
}

void GpuWorker::Initialise(bool forceGPU){
	impl_->Initialise(forceGPU);
}

unsigned int GpuWorker::NumDevices(){
	return impl_->NumDevices();
}

std::string GpuWorker::DeviceName(unsigned int gpuNum){
	return impl_->DeviceName();
}

size_t GpuWorker::MemCapacity(unsigned int gpuNum){
	return impl_->MemCapacity();
}

bool GpuWorker::BufferData(size_t bytes, bool forcedCopy, bool blocking, unsigned int gpuNum){
	return impl_->BufferData(bytes, forcedCopy, blocking, gpuNum);
}

bool GpuWorker::ReleaseData(size_t bytes, bool forceCopy, bool blocking, unsigned int gpuNum){
	return impl_->ReleaseData(bytes, forceCopy, blocking, gpuNum);
}

void GpuWorker::RunParallel(double numThreads, double opsPerThread, unsigned int gpuNum){

	this->impl_->RunParallel((unsigned int)numThreads, (unsigned int)opsPerThread, gpuNum);
}

void GpuWorker::FFT(std::vector<float> &data, unsigned int gpuNum){
	this->impl_->PerformFFT_SP(data.data(), data.size()*sizeof(float), gpuNum);
}

void GpuWorker::MatrixMultLazy(unsigned int n, unsigned int gpuNum){
	this->impl_->MatrixMult(n, gpuNum);
}

bool GpuWorker::MatrixMult(const std::vector<float> &matrixA, const std::vector<float> &matrixB,
						std::vector<float> &matrixC, unsigned int gpuNum){
	return this->impl_->MatrixMult(matrixA.size(), matrixB.size(), matrixC.size(),
								   matrixA.data(), matrixB.data(), matrixC.data(), gpuNum);
}
