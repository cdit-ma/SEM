#include "common.h"

void PrintInfo(std::string str){
    std::cerr << "[   INFO   ] " << str << std::endl;
};


std::vector<DeviceParam> getDevices(){
    Component component("dummy_component");
    Worker worker(component, "dummy_worker", "dummy_worker");

    auto platforms = OpenCLManager::GetPlatforms(worker);
    std::vector<DeviceParam> device_structs;

    for(int platform_index = 0; platform_index < platforms.size(); platform_index ++){
        auto manager = OpenCLManager::GetReferenceByPlatformID(worker, platform_index);
        if(manager){
            auto& devices = manager->GetDevices(worker);

            for(int device_index = 0; device_index < devices.size(); device_index ++){
                device_structs.emplace_back(platform_index, device_index);
            }
        }
    }
    return device_structs;
};

std::ostream& operator<<(std::ostream& os, const DeviceParam& d){
    return os << "Platform: [" << GetPlatformName(d.platform_id) << "] Device: [" << GetDeviceName(d.platform_id, d.device_id) << "]";
};

std::string GetDeviceName(int platform_id, int device_id){
    Component c("");
	Worker w(c, "", "");
    auto manager = OpenCLManager::GetReferenceByPlatformID(w, platform_id);
    if(manager){
        int id = 0;
        for (auto& device : manager->GetDevices(w)){
            if(id++ == device_id){
                std::cerr << "getting device name in tester" << std::endl;
                return device->GetName();
            }
        }
	}
    return std::to_string(device_id);
}

std::string GetPlatformName(int platform_id){
    Component c("");
    Worker w(c, "", "");
    int id = 0;
    for (auto& platform : OpenCLManager::GetPlatforms(w)){
        if(id++ == platform_id){
            return platform.getInfo<CL_PLATFORM_NAME>();
        }
    }
    return std::to_string(platform_id);
}

std::vector<float> CPUMatrixMult(float* matrix_a, float* matrix_b, size_t m, size_t k, size_t n){
    std::vector<float> matrix_c(m * n);

    for (size_t col = 0; col < n; col++) {
		for (size_t row = 0; row < m; row++) {
			
			float accum = 0;
			for (size_t t = 0; t < k; t++) {
				accum += matrix_a[t + row*k] * matrix_b[col + t*n];
			}
            matrix_c[col + row*n] = accum;
		}
	}
    return matrix_c;
};
