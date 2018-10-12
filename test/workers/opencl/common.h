#ifndef TEST_WORKERS_OPENCL_COMMON_H
#define TEST_WORKERS_OPENCL_COMMON_H

#include "gtest/gtest.h"

#include <vector>
#include <iostream>
#include <workers/opencl/openclutilities.h>
#include <workers/opencl/openclmanager.h>
#include <workers/opencl/opencl_worker.h>

#define EPS 1e-6
#define CHECK_FLOAT(x, y, eps) (fabs(x-y)<eps)

#define EXPECT_NEAR_RELATIVE(expected, actual, thresh) \
        if (actual == 0) { \
            EXPECT_NEAR(expected, actual, thresh);\
        } else { \
            EXPECT_NEAR(fabs((expected)/(actual)), 1, thresh);\
        } \

#define EXPECT_FLOATS_NEARLY_EQ(expected, actual, thresh) \
        EXPECT_EQ(expected.size(), actual.size()) << "Array sizes differ.";\
        for (size_t idx = 0; idx < std::min(expected.size(), actual.size()); ++idx) \
        { \
            if (actual[idx] == 0) { \
                EXPECT_NEAR(expected[idx], actual[idx], thresh) << "at index: " << idx;\
            } else { \
                EXPECT_NEAR(fabs((expected[idx])/(actual[idx])), 1, thresh);\
            } \
        }

struct DeviceParam{
    DeviceParam(){};
    
    DeviceParam(int platform_id, int device_id){
        this->platform_id = platform_id;
        this->device_id = device_id;
    }
    
    int platform_id = -1;
    int device_id = -1;
};


class OpenCL_WorkerConstructor{
    public:
        OpenCL_WorkerConstructor(DeviceParam device):
            component_("component"),
            worker_(component_, "OpenCL_Worker")
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

        ~OpenCL_WorkerConstructor() {
            worker_.Terminate();
        }

        Component component_;
        OpenCL_Worker worker_;
};



extern void PrintInfo(std::string str);
extern std::vector<DeviceParam> getDevices();
extern std::ostream& operator<<(std::ostream& os, const DeviceParam& d);
extern std::vector<float> CPUMatrixMult(float* matrix_a, float* matrix_b, size_t m, size_t k, size_t n);

extern std::string GetDeviceName(int platform_id, int device_id);
extern std::string GetPlatformName(int platform_id);



#endif //TEST_WORKERS_OPENCL_COMMON_H
