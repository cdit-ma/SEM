
#include <iostream>
#include <algorithm>

#include "jsonwriter.h"

#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#include "../cl2.hpp"

// Returns with false if an error is encountered
#define clPrintErrorAndReturn(err, str) clPrintError(err, str); if (err != CL_SUCCESS)return false;


void listPlatformInfo(cl::Platform platform);
void listDeviceInfo(cl::Device device);

std::string ClDeviceTypeToString(cl_device_type device_type) {
    switch (device_type) {
        case CL_DEVICE_TYPE_DEFAULT: return "CL_DEVICE_TYPE_DEFAULT";
        case CL_DEVICE_TYPE_CPU: return "CL_DEVICE_TYPE_CPU";
        case CL_DEVICE_TYPE_GPU: return "CL_DEVICE_TYPE_GPU";
        case CL_DEVICE_TYPE_ACCELERATOR: return "CL_DEVICE_TYPE_ACCELERATOR";
        case CL_DEVICE_TYPE_ALL: return "CL_DEVICE_TYPE_ALL";
        default: {
            std::cerr << "Attempting to return name string for an unknown device type" << std::endl;
            return "UNKNOWN CL_DEVICE_TYPE";
        }
    }
}



std::vector<cl::Platform> platforms;

JsonWriter jsonWriter;


int main(int argc, char* argv[]) {

    cl::Platform::get(&platforms);

    jsonWriter.Begin();
    jsonWriter.BeginArray("platforms");
    for(auto platform: platforms){
        listPlatformInfo(platform);
    }
    jsonWriter.EndArray();
    jsonWriter.End();

    //jsonWriter.begin();
   // for_each(platforms.begin(), platforms.end(), listPlatformInfo);
   // jsonWriter.end();

    std::cout << jsonWriter.ToString() << std::endl;
}


void listPlatformInfo(cl::Platform platform) {
    jsonWriter.BeginObject();
    jsonWriter.AddPair("name", platform.getInfo<CL_PLATFORM_NAME>());
    jsonWriter.AddPair("vendor", platform.getInfo<CL_PLATFORM_VENDOR>());
    jsonWriter.AddPair("version", platform.getInfo<CL_PLATFORM_VERSION>());

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    //cout << "Found platform with " << devices.size() << " devices:" << endl;
    
    jsonWriter.BeginArray("devices");
    for_each(devices.begin(), devices.end(), listDeviceInfo);
    jsonWriter.EndArray();
    jsonWriter.EndObject();
}

void listDeviceInfo(cl::Device device) {
    jsonWriter.BeginObject();
    jsonWriter.AddPair("available", std::to_string(device.getInfo<CL_DEVICE_AVAILABLE>()));
    jsonWriter.AddPair("mem_size", std::to_string(device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>()));
    jsonWriter.AddPair("max_clock_frequency", std::to_string(device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>()));
    jsonWriter.AddPair("name", device.getInfo<CL_DEVICE_NAME>());
    jsonWriter.AddPair("type", ClDeviceTypeToString(device.getInfo<CL_DEVICE_TYPE>()));
    jsonWriter.AddPair("driver_version", device.getInfo<CL_DRIVER_VERSION>());
    jsonWriter.EndObject();
}



