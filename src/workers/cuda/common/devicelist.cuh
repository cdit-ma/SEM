//
// Created by Jackson Michael on 3/5/20.
//

#ifndef RE_DEVICELIST_CUH
#define RE_DEVICELIST_CUH

#include <vector>

namespace cditma {
namespace cuda {


class DeviceList {
public:
    DeviceList();
    const int* GetDevices() const;
    int* GetMutableDevices();
    size_t Size() const;
private:
    std::vector<int> cuda_devices_;
};

} }

#endif // RE_DEVICELIST_CUH
