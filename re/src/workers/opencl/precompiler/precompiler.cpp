#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

#include "../utilities.h"

using namespace Re::OpenCL;

void precompile(cl::Device& device, std::string platform_name)
{
    cl_int err;

    std::string device_name = device.getInfo<CL_DEVICE_NAME>();
    std::cout << "Compiling for " << device_name << "..." << std::endl;

    cl::Context context(device);

    std::vector<std::string> source_files;
    source_files.push_back(GetSourcePath("precompiled.cl"));
    cl::Program::Sources source_list;
    try {
        source_list = ReadOpenCLSourceCode(source_files);
    } catch(const std::runtime_error& re) {
        std::cerr << re.what() << std::endl;
        std::cerr << "Error: Unable to read OpenCL source code, skipping compilation..." << std::endl;
        return;
    }

    cl::Program program(context, source_list);

    std::string source_path = "-I" + GetSourcePath("");
    err = program.build(source_path.c_str());
    if(err != CL_SUCCESS) {
        std::cerr << "An error occurred while building precompiled kernel: " << OpenCLErrorName(err) << std::endl;
        if(err == CL_BUILD_PROGRAM_FAILURE) {
            std::string build_output = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
            std::cerr << build_output << std::endl;
        }
        return;
    }

    cl::Program::Binaries binaries = program.getInfo<CL_PROGRAM_BINARIES>(&err);
    if(err != CL_SUCCESS) {
        std::cerr << "An error occurred while retrieving compiled binary data: " << OpenCLErrorName(err) << std::endl;
        return;
    }

    if(binaries.size() != 1) {
        std::cerr << "expected program compilation to produce one binary, " + std::to_string(binaries.size())
                         + " were produced, skipping"
                  << std::endl;
        return;
    }

    std::cout << "Compiled a binary containing the following kernels:" << std::endl;
    std::vector<cl::Kernel> kernels;
    program.createKernels(&kernels);
    for(auto& kernel : kernels) {
        std::cout << " - " << kernel.getInfo<CL_KERNEL_FUNCTION_NAME>() << std::endl;
    }

    std::string binaries_folder_path = GetSourcePath("/binaries");
    if(!boost::filesystem::exists(binaries_folder_path)) {
        boost::system::error_code error_code;
        if(!boost::filesystem::create_directory(binaries_folder_path, error_code)) {
            std::cerr << "failed to create OpenCL binaries folder: " << error_code.message() << std::endl;
            return;
        }
    }

    std::string dev_name = SanitisePathString(device_name).substr(0, 15);
    std::string plat_name = SanitisePathString(platform_name).substr(0, 15);

    std::string filename = plat_name + "-" + dev_name + ".clbin";

    //#ifdef _WIN32
    //    std::string binary_filepath = GetSourcePath("binaries\\"+platform_name+"-"+device_name+".clbin");
    //#else
    std::string binary_filepath = GetSourcePath("binaries/" + filename);
    //#endif
    std::ofstream binary_stream(binary_filepath, std::ios::out | std::ios::binary);

    if(!binary_stream.is_open()) {
        std::cerr << "unable to open file " + binary_filepath + " for writing; unable to produce binary" << std::endl;
        return;
    }

    size_t binary_length = binaries.at(0).size() * sizeof(cl::Program::Binaries::value_type::value_type);
    binary_stream.write((char*)binaries.at(0).data(), binary_length);

    binary_stream.close();

    std::cout << "Binary file written to " << binary_filepath << std::endl;
}

int main(int argc, char** argv)
{
    std::cout << "Beginning precompilation of OpenCL kernels..." << std::endl;
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    for(auto& platform : platforms) {
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
        for(auto& device : devices) {
            precompile(device, platform.getInfo<CL_PLATFORM_NAME>());
        }
    }
}
