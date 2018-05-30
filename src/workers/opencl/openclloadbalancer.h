#ifndef OPENCLLOADBALANCER
#define OPENCLLOADBALANCER

#include <vector>
#include <set>
#include <iostream>

#include <mutex>

class OpenCLLoadBalancer {
public:
    OpenCLLoadBalancer(const std::vector<unsigned int>& device_ids);

    unsigned int RequestDevice();
    void ReleaseDevice(unsigned int device_id);


private:

    class DeviceJobList {
    public:
        unsigned int device_id_;
        unsigned int num_jobs_;
        DeviceJobList(unsigned int dev_id) : device_id_(dev_id), num_jobs_(0){};
        bool operator<(const DeviceJobList& rhs) const {
            if ( num_jobs_ == rhs.num_jobs_) {
                return device_id_ < rhs.device_id_;
            }
            return num_jobs_ < rhs.num_jobs_;
        }
        bool operator()(const DeviceJobList& lhs, const DeviceJobList& rhs) const {
            return lhs < rhs;
        }
    };

    std::set<DeviceJobList> device_ids_;

    std::mutex mutex_;
};

#endif