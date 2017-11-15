#ifndef OPENCLWORKER_H
#define OPENCLWORKER_H

#include <core/worker.h>
#include "openclmanager.h"
#include "oclbuffer.hpp"
#include "openclkernel.hpp"
#include "openclloadbalancer.h"

class OpenCLWorker : public Worker {
public:
    OpenCLWorker(Component* component, std::string inst_name);
    ~OpenCLWorker();

    bool Configure(int platform_id=0, int device_id=0);
    bool IsValid() const;

    // Base/Utility functions
    template <typename T>
    OCLBuffer<T>* CreateBuffer(std::vector<T> data, bool blocking=true);
    template <typename T>
    void ReleaseBuffer(OCLBuffer<T>* buffer);
    template <typename T>
    bool WriteBuffer(OCLBuffer<T>& buffer, const std::vector<T>& data, bool blocking=true);
    template <typename T>
    std::vector<T> ReadBuffer(const OCLBuffer<T>& buffer, bool blocking=true);

    // Bespoke algorithms
    bool RunParallel(int num_threads, long long ops_per_thread);
    bool MatrixMult(const OCLBuffer<float>& matA, const OCLBuffer<float>& matB, OCLBuffer<float>& matC);
    bool MatrixMult(const std::vector<float>& matA, const std::vector<float>& matB, std::vector<float>& matC);
    bool KmeansCluster(const OCLBuffer<float>& points, OCLBuffer<float>& centroids, OCLBuffer<int>& point_classifications, int iterations);
    bool KmeansCluster(const std::vector<float>& points, std::vector<float>& centroids, std::vector<int>& point_classifications, int iterations);


protected:
    virtual void Log(std::string function_name, ModelLogger::WorkloadEvent event, int work_id = -1, std::string args = "");


private:

    OpenCLKernel* InitKernel(OpenCLManager& manager, std::string kernel_name, std::string source_file);

    bool is_valid_ = false;

    OpenCLManager* manager_ = NULL;
    OpenCLLoadBalancer* load_balancer_ = NULL;
    
    OpenCLKernel* parallel_kernel_ = NULL;
    OpenCLKernel* matrix_kernel_ = NULL;
    OpenCLKernel* cluster_classify_kernel_ = NULL;
    OpenCLKernel* cluster_adjust_kernel_ = NULL;

};


template <typename T>
OCLBuffer<T>* OpenCLWorker::CreateBuffer(std::vector<T> data, bool blocking) {
    return manager_->CreateBuffer(data);
}

template <typename T>
void OpenCLWorker::ReleaseBuffer(OCLBuffer<T>* buffer) {
    return manager_->ReleaseBuffer(buffer);
}

template <typename T>
bool OpenCLWorker::WriteBuffer(OCLBuffer<T>& buffer, const std::vector<T>& data, bool blocking) {
    return buffer.WriteData(data, blocking, this);
}

template <typename T>
std::vector<T> OpenCLWorker::ReadBuffer(const OCLBuffer<T>& buffer, bool blocking) {
    return buffer.ReadData(blocking, this);
}

#endif