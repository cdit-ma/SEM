#ifndef RE_OPENCL_MANAGER_H
#define RE_OPENCL_MANAGER_H

#include <map>
#include <mutex>
#include <optional>
#include <unordered_map>

#include "logger.h"
#include "worker.h"

#include "buffer.hpp"
#include "device.h"

namespace cl {
class Platform;
class Context;
class CommandQueue;
class Program;
class Kernel;
} // namespace cl

class OpenCL_Worker;

namespace Re {

namespace OpenCL {

class Kernel;

class Manager {
    public:
    ~Manager();

    /**
     * Returns the Manager responsible for managing a given OpenCL platform,
     * constructing and initializing the appropriate resources if required.
     *
     * @param platformID the index of the OpenCL platform as specified by the deployment information
     * @param workerReference a reference to the worker making the call (for logging)
     * @return The Manager for the provided platform, or NULL if one can't be created
     **/
    static Manager* GetReferenceByPlatformID(int platform_id, Worker* worker_ref = nullptr);

    static Manager* GetReferenceByPlatformName(std::string platform_name, Worker* worker_ref = nullptr);

    static const std::vector<cl::Platform> GetPlatforms();

    const cl::Context& GetContext() const;

    std::string GetPlatformName() const;

    const std::vector<std::unique_ptr<Device>>& GetDevices();

    // const std::vector<std::shared_ptr<cl::CommandQueue>> GetQueues() const;

    const std::vector<Kernel> CreateKernels(const std::vector<std::string>& filenames);

    template<typename T> Buffer<T> CreateBuffer(size_t buffer_size) const;
    template<typename T> Buffer<T> CreateBuffer(const std::vector<T>& data, Device& device, bool blocking = true) const;

    template<typename T> void ReleaseBuffer(Buffer<T>& buffer);

    bool IsValid() const;
    bool IsFPGA() const;

    class BufferAttorney {
        BufferAttorney() = delete;

        private:
        friend class BufferBase;
        static int GetNewBufferID(Manager& manager, BufferBase& buffer) { return manager.TrackBuffer(buffer); }
        static void ReleaseBufferID(Manager& manager, BufferBase& buffer) { manager.UntrackBuffer(buffer.GetID()); }
    };

    static const int invalid_buffer_id_ = -1;

    private:
    Manager(cl::Platform& platform, Worker* worker_ref);

    int TrackBuffer(const BufferBase& buffer);
    void UntrackBuffer(int buffer_id);
    void Initialise();

    bool LoadAllBinaries();

    static void UpdatePlatformList();

    static void LogError(const Worker& worker, std::string function_name, std::string error_message, int cl_error_code);
    static void LogError(const Worker& worker, std::string function_name, std::string error_message);
    static void LogMessage(const Worker& worker, std::string function_name, std::string message);

    // A separate Manager is maintained for each OpenCL platform
    static std::unordered_map<unsigned int, std::unique_ptr<Manager>> reference_map_;
    static std::vector<cl::Platform> platform_list_;

    // Variables
    bool valid_ = false;
    cl::Platform& platform_;
    std::string platform_name_;
    std::unique_ptr<cl::Context> context_;
    std::vector<std::unique_ptr<Device>> device_list_;
    // std::vector<std::shared_ptr<cl::CommandQueue>> queues_;

    std::mutex opencl_resource_mutex_;

    std::mutex opencl_buffer_mutex_;
    std::map<int, size_t> buffer_store_;
    int buffer_id_count_ = -1;

    bool is_fpga_ = false;
};

template<typename T> Buffer<T> Manager::CreateBuffer(size_t buffer_size) const { return Buffer<T>(*this, buffer_size); }

template<typename T> Buffer<T> Manager::CreateBuffer(const std::vector<T>& data, Device& device, bool blocking) const
{
    return Buffer<T>(*this, data, device, blocking);
}

template<typename T> void Manager::ReleaseBuffer(Buffer<T>& buffer) { buffer.Release(*this); }

} // namespace OpenCL

} // namespace Re

#endif // OPENCL_MANAGER_H
