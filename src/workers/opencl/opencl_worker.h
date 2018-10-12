#ifndef OpenCL_Worker_H
#define OpenCL_Worker_H

#include <core/worker.h>
#include <core/component.h>
#include "openclmanager.h"
#include "openclbuffer.hpp"
#include "openclkernel.hpp"
#include "openclloadbalancer.h"


struct clfftSetupData_;
struct cl_comand_queue;

class OpenCL_Worker : public Worker {
public:
    OpenCL_Worker(const BehaviourContainer& bc, std::string inst_name);
    //~OpenCL_Worker();

    void HandleConfigure() override;
    void HandleTerminate() override;
    bool IsValid() const;

    // Base/Utility functions
    template <typename T>
    OpenCLBuffer<T> CreateBuffer(std::vector<T> data, bool blocking=true);
    template <typename T>
    void ReleaseBuffer(OpenCLBuffer<T>& buffer);
    template <typename T>
    bool WriteBuffer(OpenCLBuffer<T>& buffer, const std::vector<T>& data, bool blocking=true);
    template <typename T>
    std::vector<T> ReadBuffer(const OpenCLBuffer<T>& buffer, bool blocking=true);
    template <typename T>
    bool IsBufferValid(const OpenCLBuffer<T>& buffer) const;

    // Bespoke algorithms
    bool RunParallel(int num_threads, long long ops_per_thread);
    bool MatrixMult(const OpenCLBuffer<float>& matA, const OpenCLBuffer<float>& matB, OpenCLBuffer<float>& matC, OpenCLDevice& device);
    bool MatrixMult(const std::vector<float>& matA, const std::vector<float>& matB, std::vector<float>& matC);
    bool KmeansCluster(const OpenCLBuffer<float>& points, OpenCLBuffer<float>& centroids, OpenCLBuffer<int>& point_classifications, int iterations);
    bool KmeansCluster(const std::vector<float>& points, std::vector<float>& centroids, std::vector<int>& point_classifications, int iterations);

    // FFT function implementation to be conditionally compiled based on the presence of the required FFT libraries
    bool FFT(std::vector<float> &data);
    bool FFT(OpenCLBuffer<float> &data);



protected:
    virtual void Log(std::string function_name, ModelLogger::WorkloadEvent event, int work_id = -1, std::string args = "");

    bool InitFFT();
    bool CleanupFFT();


private:
    // Can throw if source file doesn't exist, contains no kernels or doesn't contain the specified kernel
    OpenCLKernel& GetKernel(OpenCLDevice& device, const std::string& kernel_name, const std::string& source_file);

    bool is_valid_ = false;

    OpenCLManager* manager_ = NULL;
    OpenCLLoadBalancer* load_balancer_ = NULL;

    std::vector<std::reference_wrapper<OpenCLDevice> > devices_;

    std::shared_ptr<Attribute> platform_id_;
    std::shared_ptr<Attribute> device_id_;

    // clFFT specific members
    clfftSetupData_* fftSetupData = 0;

    // FPGA FFT specific members
    std::vector<OpenCLQueue> fetch_queues_;
    OpenCLKernel* fpga_fft_kernel_ = 0;
    OpenCLKernel* fpga_fetch_kernel_ = 0;
};


template <typename T>
OpenCLBuffer<T> OpenCL_Worker::CreateBuffer(std::vector<T> data, bool blocking) {
    OpenCLBuffer<T> new_buffer ;
    try {
        new_buffer = manager_->CreateBuffer<T>(*this, data.size());
        new_buffer.Track(*this, *manager_);
        WriteBuffer(new_buffer, data, blocking);
    } catch (const std::exception& e) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            std::string("Unable to create an OpenCL buffer from a vector:\n")+e.what());
    }
    return new_buffer;
}

template <typename T>
void OpenCL_Worker::ReleaseBuffer(OpenCLBuffer<T>& buffer) {
    try {
        //buffer.Release(*manager_);
        buffer.Untrack(*manager_);
        manager_->ReleaseBuffer(*this, buffer);
    } catch (const std::exception& e) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            std::string("Unable to release an OpenCL buffer:\n")+e.what());
    }
    //return manager_->ReleaseBuffer(*this, buffer);
}

template <typename T>
bool OpenCL_Worker::WriteBuffer(OpenCLBuffer<T>& buffer, const std::vector<T>& data, bool blocking) {
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
std::vector<T> OpenCL_Worker::ReadBuffer(const OpenCLBuffer<T>& buffer, bool blocking) {
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

template <typename T>
bool OpenCL_Worker::IsBufferValid(const OpenCLBuffer<T>& buffer) const {
    return buffer.IsValid();
}

#endif
