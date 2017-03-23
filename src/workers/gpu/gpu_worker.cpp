// WE_GPU.mpc  2015-08-17  Jackson Michael 

#include "gpu_worker_impl.h" 
#include "gpu_worker.h"
#include <iostream>
#include <sstream>
#include <core/component.h>
#include <core/modellogger.h>

/**
 * The WE_GPU class acts is a wrapper for WE_GPU_Impl, allowing projects to be compiled
 * without having to link against the dependencies requied by the proper GPU worker
 * implementation.
 */

Gpu_Worker::Gpu_Worker(Component* component, std::string inst_name) : Worker(component, __func__, inst_name){
    impl_ = new Gpu_Worker_Impl();
	impl_->Initialise(false);
}

Gpu_Worker::~Gpu_Worker(void){
	delete impl_;
}

void Gpu_Worker::Release(){
	impl_->Release();
}

void Gpu_Worker::Initialise(bool forceGPU){
	impl_->Initialise(forceGPU);
}

unsigned int Gpu_Worker::NumDevices(){
	return impl_->NumDevices();
}

std::string Gpu_Worker::DeviceName(unsigned int gpuNum){
	return impl_->DeviceName();
}

size_t Gpu_Worker::MemCapacity(unsigned int gpuNum){
	return impl_->MemCapacity();
}

bool Gpu_Worker::BufferData(size_t bytes, bool forcedCopy, bool blocking, unsigned int gpuNum){
	return impl_->BufferData(bytes, forcedCopy, blocking, gpuNum);
}

bool Gpu_Worker::ReleaseData(size_t bytes, bool forceCopy, bool blocking, unsigned int gpuNum){
	return impl_->ReleaseData(bytes, forceCopy, blocking, gpuNum);
}

void Gpu_Worker::RunParallel(double numThreads, double opsPerThread, unsigned int gpuNum){
	auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("numThreads = %d, opsPerThread = %d, on gpu = %d", numThreads, opsPerThread, gpuNum);

    //Log Before
    Log(fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
	impl_->RunParallel((unsigned int)numThreads, (unsigned int)opsPerThread, gpuNum);
	Log(fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
}

void Gpu_Worker::FFT(std::vector<float> &data, unsigned int gpuNum){
	auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("FFT size = %d on gpu = %d", data.size(), gpuNum);

    //Log Before
    Log(fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
	impl_->PerformFFT_SP(data.data(), data.size()*sizeof(float), gpuNum);
    Log(fun, ModelLogger::WorkloadEvent::FINISHED, work_id);	
}

void Gpu_Worker::MatrixMultLazy(unsigned int n, unsigned int gpuNum){
	auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("matrix dimensions = %d on gpu = %d", n, gpuNum);

    //Log Before
    Log(fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
	impl_->MatrixMult(n, gpuNum);
	Log(fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
}

bool Gpu_Worker::MatrixMult(const std::vector<float> &matrixA, const std::vector<float> &matrixB,
						std::vector<float> &matrixC, unsigned int gpuNum){

	auto work_id = get_new_work_id();
    auto fun = std::string(__func__);
    auto args = get_arg_string_variadic("matAsize = %d, matBsize = %d, matCsize = %d, on gpu = %d",
										 matrixA.size(), matrixB.size(), matrixC.size(), gpuNum);

    //Log Before
    Log(fun, ModelLogger::WorkloadEvent::STARTED, work_id, args);
	auto res = impl_->MatrixMult(matrixA.size(), matrixB.size(), matrixC.size(),
	  							matrixA.data(), matrixB.data(), matrixC.data(), gpuNum);
	Log(fun, ModelLogger::WorkloadEvent::FINISHED, work_id);
	return res;
	
}
