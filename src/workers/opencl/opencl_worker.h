#ifndef RE_OPENCL_WORKER_H
#define RE_OPENCL_WORKER_H

#include <core/component.h>
#include <core/worker.h>
#include <optional>

#include "buffer.hpp"
#include "fftprovider.h"
#include "kernel.hpp"
#include "loadbalancer.h"
#include "manager.h"

struct clfftSetupData_;
struct cl_comand_queue;

class OpenCL_Worker : public Worker {
    public:
    OpenCL_Worker(const BehaviourContainer& bc, std::string inst_name);

    void HandleConfigure() override;
    void HandleTerminate() override;
    bool IsValid() const;
    const std::string& get_version() const override;

    // Base/Utility functions
    template<typename T> Re::OpenCL::Buffer<T> CreateBuffer(std::vector<T> data, bool blocking = true);
    template<typename T> void ReleaseBuffer(Re::OpenCL::Buffer<T>& buffer);
    template<typename T>
    bool WriteBuffer(Re::OpenCL::Buffer<T>& buffer, const std::vector<T>& data, bool blocking = true);
    template<typename T> std::vector<T> ReadBuffer(const Re::OpenCL::Buffer<T>& buffer, bool blocking = true);
    template<typename T> bool IsBufferValid(const Re::OpenCL::Buffer<T>& buffer) const;

    // Bespoke algorithms
    bool RunParallel(int num_threads, long long ops_per_thread);
    bool MatrixMult(const Re::OpenCL::Buffer<float>& matA,
                    const Re::OpenCL::Buffer<float>& matB,
                    Re::OpenCL::Buffer<float>& matC,
                    Re::OpenCL::Device& device,
                    int work_id = -1);
    bool MatrixMult(const std::vector<float>& matA, const std::vector<float>& matB, std::vector<float>& matC);
    bool KmeansCluster(const Re::OpenCL::Buffer<float>& points,
                       Re::OpenCL::Buffer<float>& centroids,
                       Re::OpenCL::Buffer<int>& point_classifications,
                       int iterations,
                       int work_id = -1);
    bool KmeansCluster(const std::vector<float>& points,
                       std::vector<float>& centroids,
                       std::vector<int>& point_classifications,
                       int iterations);

    // FFT function implementation to be conditionally compiled based on the presence of the required FFT libraries
    bool FFT(std::vector<float>& data);
    bool FFT(Re::OpenCL::Buffer<float>& data, int device_id = -1, int work_id = -1);

    private:
    // Can throw if source file doesn't exist, contains no kernels or doesn't contain the specified kernel
    Re::OpenCL::Kernel&
    GetKernel(Re::OpenCL::Device& device, const std::string& kernel_name, const std::string& source_file);

    std::shared_ptr<Attribute> platform_id_;
    std::shared_ptr<Attribute> device_id_;

    bool is_valid_ = false;

    Re::OpenCL::Manager* manager_ = NULL;
    std::unique_ptr<Re::OpenCL::LoadBalancer> load_balancer_ = NULL;

    std::vector<std::reference_wrapper<Re::OpenCL::Device>> devices_;

    std::unique_ptr<Re::OpenCL::FFTProvider> fft_provider_;
};

template<typename T> Re::OpenCL::Buffer<T> OpenCL_Worker::CreateBuffer(std::vector<T> data, bool blocking)
{
    Re::OpenCL::Buffer<T> new_buffer;
    try {
        new_buffer = manager_->CreateBuffer<T>(data.size());
        new_buffer.Track(*manager_);
        WriteBuffer(new_buffer, data, blocking);
    } catch(const std::exception& e) {
        Log(GET_FUNC, Logger::WorkloadEvent::ERROR, get_new_work_id(),
            std::string("Unable to create an OpenCL buffer from a vector: ") + e.what());
    }
    return new_buffer;
}

template<typename T> void OpenCL_Worker::ReleaseBuffer(Re::OpenCL::Buffer<T>& buffer)
{
    try {
        buffer.Untrack(*manager_);
        manager_->ReleaseBuffer(buffer);
    } catch(const std::exception& e) {
        Log(GET_FUNC, Logger::WorkloadEvent::ERROR, get_new_work_id(),
            std::string("Unable to release an OpenCL buffer: ") + e.what());
    }
}

template<typename T>
bool OpenCL_Worker::WriteBuffer(Re::OpenCL::Buffer<T>& buffer, const std::vector<T>& data, bool blocking)
{
    if(devices_.size() == 0) {
        Log(GET_FUNC, Logger::WorkloadEvent::ERROR, get_new_work_id(),
            "Cannot write to buffer when worker has no associated devices");
        return false;
    }

    if(devices_.size() > 1) {
        Log(GET_FUNC, Logger::WorkloadEvent::WARNING, get_new_work_id(),
            "Attempting to write to buffer using a worker that has multiple associated devices");
    }

    bool did_all_succeed = true;
    for(const auto& dev_wrapper : devices_) {
        bool success = buffer.WriteData(data, dev_wrapper.get(), blocking);
        if(!success) {
            Log(GET_FUNC, Logger::WorkloadEvent::ERROR, get_new_work_id(),
                "Failed to write to Re::OpenCL::Buffer for device " + dev_wrapper.get().GetName());
            did_all_succeed = false;
        }
    }
    return did_all_succeed;
}

template<typename T> std::vector<T> OpenCL_Worker::ReadBuffer(const Re::OpenCL::Buffer<T>& buffer, bool blocking)
{
    if(!IsBufferValid(buffer)) {
        Log(GET_FUNC, Logger::WorkloadEvent::ERROR, get_new_work_id(),
            "Cannot read from buffer when buffer is invalid");
        return std::vector<T>();
    }

    if(devices_.size() == 0) {
        Log(GET_FUNC, Logger::WorkloadEvent::ERROR, get_new_work_id(),
            "Cannot read from buffer when worker has no associated devices");
        return std::vector<T>();
    }

    if(devices_.size() > 1) {
        Log(GET_FUNC, Logger::WorkloadEvent::ERROR, get_new_work_id(),
            "Cannot read buffer using a worker that has multiple associated devices");
        return std::vector<T>();
    }

    try {
        return buffer.ReadData(devices_.at(0), blocking);
    } catch(const std::exception& e) {
        Log(GET_FUNC, Logger::WorkloadEvent::ERROR, get_new_work_id(),
            std::string("An error occurred while calling ReadBuffer:\n") + e.what());
        return std::vector<T>();
    }
}

template<typename T> bool OpenCL_Worker::IsBufferValid(const Re::OpenCL::Buffer<T>& buffer) const
{
    return buffer.IsValid();
}

#endif // RE_OPENCL_WORKER_H
