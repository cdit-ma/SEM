#ifndef TEST_WORKERS_OPENCL_COMMON_H
#define TEST_WORKERS_OPENCL_COMMON_H

#include "gtest/gtest.h"

#include <vector>
#include <iostream>
#include <workers/opencl/openclutilities.h>
#include <workers/opencl/openclmanager.h>
#include <workers/opencl/opencl_worker.h>

#define EPS 1e-7
#define CHECK_FLOAT(x, y, eps) (fabs(x-y)<eps)

struct DeviceParam{
    DeviceParam(){};
    
    DeviceParam(int platform_id, int device_id){
        this->platform_id = platform_id;
        this->device_id = device_id;
    }
    
    int platform_id = -1;
    int device_id = -1;
};




class OpenCLWorkerConstructor{
    public:
        OpenCLWorkerConstructor(DeviceParam device):
            component_("component"),
            worker_(component_, "openclworker")
        {
            auto platform_attr = worker_.GetAttribute("platform_id").lock();
            if (platform_attr) {
                platform_attr->set_Integer(device.platform_id);
            }
            auto device_attr = worker_.GetAttribute("device_id").lock();
            if (device_attr) {
                device_attr->set_Integer(device.device_id);
            }
        }
        Component component_;
        OpenCLWorker worker_;
};



extern void PrintInfo(std::string str);
extern std::vector<DeviceParam> getDevices();
extern std::ostream& operator<<(std::ostream& os, const DeviceParam& d);
extern std::vector<float> CPUMatrixMult(float* matrix_a, float* matrix_b, size_t m, size_t k, size_t n);

extern std::string GetDeviceName(int platform_id, int device_id);
extern std::string GetPlatformName(int platform_id);



#endif //TEST_WORKERS_OPENCL_COMMON_H