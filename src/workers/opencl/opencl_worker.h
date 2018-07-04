#ifndef OPENCLWORKER_H
#define OPENCLWORKER_H

#include <core/worker.h>
#include <core/component.h>
#include "openclmanager.h"
#include "oclbuffer.hpp"
#include "openclkernel.hpp"
#include "openclloadbalancer.h"

class OpenCLWorker : public Worker {
public:
    OpenCLWorker(const Component& component, std::string inst_name);
    ~OpenCLWorker();

    bool Configure();
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
    bool MatrixMult(const OCLBuffer<float>& matA, const OCLBuffer<float>& matB, OCLBuffer<float>& matC, OpenCLDevice& device);
    bool MatrixMult(const std::vector<float>& matA, const std::vector<float>& matB, std::vector<float>& matC);
    bool KmeansCluster(const OCLBuffer<float>& points, OCLBuffer<float>& centroids, OCLBuffer<int>& point_classifications, int iterations);
    bool KmeansCluster(const std::vector<float>& points, std::vector<float>& centroids, std::vector<int>& point_classifications, int iterations);


protected:
    virtual void Log(std::string function_name, ModelLogger::WorkloadEvent event, int work_id = -1, std::string args = "");


private:

    OpenCLKernel* InitKernel(OpenCLManager& manager, std::string kernel_name, std::string source_file);
    // Can throw if source file doesn't exist, contains no kernels or doesn't contain the specified kernel
    OpenCLKernel& GetKernel(OpenCLDevice& device, const std::string& kernel_name, const std::string& source_file);

    bool is_valid_ = false;

    OpenCLManager* manager_ = NULL;
    OpenCLLoadBalancer* load_balancer_ = NULL;

    std::vector<std::reference_wrapper<OpenCLDevice> > devices_;

    std::shared_ptr<Attribute> platform_id_;
    std::shared_ptr<Attribute> device_id_;

};


template <typename T>
OCLBuffer<T>* OpenCLWorker::CreateBuffer(std::vector<T> data, bool blocking) {
    OCLBuffer<T>* new_buffer = manager_->CreateBuffer<T>(*this, data.size());
    /*for (const auto& dev_wrapper : devices_) {
        new_buffer->WriteData(data, dev_wrapper.get(), blocking, this);
    }*/
    WriteBuffer(*new_buffer, data, blocking);
    return new_buffer;
}

template <typename T>
void OpenCLWorker::ReleaseBuffer(OCLBuffer<T>* buffer) {
    return manager_->ReleaseBuffer(*this, buffer);
}

template <typename T>
bool OpenCLWorker::WriteBuffer(OCLBuffer<T>& buffer, const std::vector<T>& data, bool blocking) {
    if (devices_.size() == 0) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Cannot write to buffer when worker has no associated devices");
        return false;
    }

    if (devices_.size() > 1) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Attempting to write to buffer using a worker that has multiple associated devices");
    }

    bool did_all_succeed = true;
    for (const auto& dev_wrapper : devices_) {
        bool success = buffer.WriteData(*this, data, dev_wrapper.get(), blocking);
        if (!success) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "Failed to write to OpenCLBuffer for device "+dev_wrapper.get().GetName());
            did_all_succeed = false;
        }
    }
    return did_all_succeed;
}

template <typename T>
std::vector<T> OpenCLWorker::ReadBuffer(const OCLBuffer<T>& buffer, bool blocking) {
    if (devices_.size() == 0) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Cannot read from buffer when worker has no associated devices");
        return std::vector<T>();
    }

    if (devices_.size() > 1) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Cannot read buffer using a worker that has multiple associated devices");
        return std::vector<T>();
    }

    return buffer.ReadData(*this, devices_.at(0), blocking);
}

#endif