#ifndef TEST_WORKERS_OPENCL_COMMON_H
#define TEST_WORKERS_OPENCL_COMMON_H

#include "gtest/gtest.h"

#include <common/common.h>

#include <core/loggers/print_logger.h>
#include <iostream>
#include <vector>
#include <workers/opencl/manager.h>
#include <workers/opencl/opencl_worker.h>
#include <workers/opencl/utilities.h>

#define EPS 1e-6

struct DeviceParam {
    DeviceParam() = default;

    DeviceParam(int platform_id, int device_id)
    {
        this->platform_id = platform_id;
        this->device_id = device_id;
    }

    int platform_id = -1;
    int device_id = -1;
};

class OpenCL_WorkerConstructor {
public:
    explicit OpenCL_WorkerConstructor(DeviceParam device) :
        component_("component"),
        worker_(component_, "OpenCL_Worker")
    {
        Print::Logger::get_logger().SetLogLevel(10);
        worker_.logger().AddLogger(Print::Logger::get_logger());

        // Set the logger

        auto platform_attr = worker_.GetAttribute("platform_id").lock();
        if(platform_attr) {
            platform_attr->set_Integer(device.platform_id);
        }
        auto device_attr = worker_.GetAttribute("device_id").lock();
        if(device_attr) {
            device_attr->set_Integer(device.device_id);
        }
    }

    ~OpenCL_WorkerConstructor() { worker_.Terminate(); }

    Component component_;
    OpenCL_Worker worker_;
};

extern void PrintInfo(std::string str);
extern std::vector<DeviceParam> getDevices();
extern std::ostream& operator<<(std::ostream& os, const DeviceParam& d);
extern std::vector<float>
CPUMatrixMult(float* matrix_a, float* matrix_b, size_t m, size_t k, size_t n);

extern std::string GetDeviceName(int platform_id, int device_id);
extern std::string GetPlatformName(int platform_id);

class DummyWorker : public Worker {
public:
    explicit DummyWorker(Component& component);
    const std::string& get_version() const override;
};

#endif // TEST_WORKERS_OPENCL_COMMON_H
