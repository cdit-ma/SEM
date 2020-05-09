
#include <algorithm>
#include <iostream>

#include "jsonwriter.h"
#include <cuda/api_wrappers.hpp>
#include <cuda_runtime.h>

using namespace cuda;

void listPlatformInfo(JsonWriter& jsonWriter);
void listDeviceInfo(JsonWriter& jsonWriter, const device_t& device);

int main(int argc, char* argv[])
{
    JsonWriter jsonWriter;
    jsonWriter.Begin();
    listPlatformInfo(jsonWriter);
    jsonWriter.End();

    std::cout << jsonWriter.ToString() << std::endl;
}

void listPlatformInfo(JsonWriter& jsonWriter)
{
    jsonWriter.BeginKeyedObject("cuda");
    // jsonWriter.AddPair("name", platform.getInfo<CL_PLATFORM_NAME>());

    std::stringstream version_sstream;
    version_sstream << cuda::version_numbers::runtime();
    jsonWriter.AddPair("runtime_version", version_sstream.str());

    jsonWriter.BeginArray("devices");
    for(device::id_t dev_id=0; dev_id < device::count(); dev_id++) {
        listDeviceInfo(jsonWriter, device::get(dev_id));
    }
    jsonWriter.EndArray();

    jsonWriter.EndObject();
}

void listDeviceInfo(JsonWriter& jsonWriter, const cuda::device_t& device)
{
    // Get the Cpp wrapper properties
    const device::properties_t&& props = device.properties();

    // Use the underlying C api to get certain missing properties like clock frequency
    cudaDeviceProp raw_properties;
    cudaGetDeviceProperties(&raw_properties, device.id());

    jsonWriter.BeginObject();
    jsonWriter.AddPair("device_id", std::to_string(device.id()));
    jsonWriter.AddPair("available", std::to_string(props.usable_for_compute()));
    jsonWriter.AddPair("mem_size", std::to_string(props.global_memory_size()));
    std::string frequency = std::to_string(static_cast<float>(raw_properties.clockRate) / 1000.0f);
    jsonWriter.AddPair("max_clock_frequency_MHz", frequency);
    jsonWriter.AddPair("name", device.name());
    std::string compute_capability = std::to_string(props.compute_capability().major()) + "."
                                     + std::to_string(props.compute_capability().minor());
    jsonWriter.AddPair("compute_capability", compute_capability);
    jsonWriter.AddPair("pci_id", props.pci_id());
    jsonWriter.EndObject();
}
