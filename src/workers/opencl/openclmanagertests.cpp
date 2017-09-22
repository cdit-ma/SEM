

#include <iostream>
#include <vector>

#include "openclmanager.h"
#include "openclkernel.hpp"

void printFail(std::string message) {
	std::cerr << "\033[31m" << message << "\033[0m" <<std::endl;
}

void testBufferReadWrite(OpenCLManager& manager);
void testKernelPassthrough(OpenCLManager& manager, OpenCLKernel& passthrough_kernel);

int main(int argc, char** argv) {

	std::cout << "Beginning OpenCL worker Tester..." << std::endl;

	// List out the avaialble platforms
	std::cout << "Identifying OpenCL platforms:" << std::endl;
	auto platforms = OpenCLManager::GetPlatforms();
	if (platforms.size() == 0) {
		printFail("Failed to find any OpenCL platforms");
		return 1;
	}
	for (auto platform : platforms) {
		std::cout << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
	}
	std::cout << std::endl;
	
	std::cout << "Attemptiong to create OpenCL manager for the first platform..." << std::endl;
	auto manager = OpenCLManager::GetReferenceByPlatform(0);
	if (manager->IsValid()) {
		for (auto device : manager->GetDevices()) {
			std::cout << device.getInfo<CL_DEVICE_NAME>() << std::endl;
		}
	}
	else {
		printFail("Failed to create OpenCLManager");
		return 1;
	}
	std::cout << std::endl;

	// Generate the list of source files
	// For now, just add the "kernel" and "matrixmult" OpenCL files in the source directory
	std::string source_file_path(__FILE__);
	auto source_dir = source_file_path.substr(0, source_file_path.find_last_of("/\\")+1);
	std::vector<std::string> filenames = {
		source_dir + "kernel.cl",
		source_dir + "matrixmult.cl"
	};

	std::cout << "Loading the following kernel source files:" << std::endl;
	for (auto fn : filenames) {
		std::cout << " - " << fn << std::endl;
	}

	// Generate the kernels for the provided source files
	auto kernels = manager->CreateKernels(filenames);
	if (kernels.size() == 0) {
		printFail("No OpenCL kernels were created");
		return 1;
	} else {
		std::cout << "Listing built kernels:" << std::endl;
		for (auto k : kernels) {
			std::cout << " - " << k.GetName() << std::endl;
		}
	}

	testBufferReadWrite(*manager);
	std::cout << std::endl;

	OpenCLKernel* pt_kernel;
	bool found_pt_kernel = false;
	for (auto& k : kernels) {
		if (k.GetName() == "dataPassthroughTest") {
			pt_kernel = &k;
			found_pt_kernel = true;
		}
	}
	if (!found_pt_kernel) {
		printFail("Unable to find dataPasshtoughTest kernel");
	}

	testKernelPassthrough(*manager, *pt_kernel);
	std::cout << std::endl;
	
	std::cout << "Finished" << std::endl;

	//system("pause");
}

void testBufferReadWrite(OpenCLManager& manager) {
	// Create a test buffer
	auto buffer = manager.CreateBuffer<float>(4);
	//OCLBuffer<std::string>* stringBuffer = manager->CreateBuffer<std::string>(1024);

	// Send some data through
	std::cout << "Writing data to a buffer (4 elements containing 0.5f)..." << std::endl;
	auto in_data = std::vector<float>(4, 0.5f);
	buffer->WriteData(in_data);

	// Read it back
	std::cout << "Reading the data back..." << std::endl;
	auto out_data = buffer->ReadData();
	bool vectors_match = true;
	for (int i=0; i<in_data.size(); i++) {
		if (in_data[i] != out_data[0]) {
			std::cout << "Mismatch at element " << i << ": " << in_data[i] << "/" << out_data[i] << std::endl;
			vectors_match = false;
		}
	}
	if (vectors_match) {
		std::cout << "Buffer read/write test successful" << std::endl;
	}

	delete buffer;
}


void testKernelPassthrough(OpenCLManager& manager, OpenCLKernel& passthrough_kernel) {
	cl_int err;

	auto in_data = std::vector<float>(4, 0.5f);
	auto in_buffer = manager.CreateBuffer<float>(in_data);
	auto out_buffer = manager.CreateBuffer<float>(4);

	std::cout << "Setting kernel arguments..." << std::endl;
	passthrough_kernel.SetArgs((*in_buffer), (*out_buffer));
	//kernel->SetArgs(*in_buffer, 2);

	std::cout << "Enqueueing kernel..." << std::endl;
	cl::Event kernelEvent;
	err = manager.GetQueues()[0].enqueueNDRangeKernel(passthrough_kernel.GetBackingRef(), 
			cl::NullRange, cl::NDRange(4), cl::NDRange(4), NULL, &kernelEvent);
	if (err != CL_SUCCESS) {
		LogOpenCLError(NULL, __func__, "Couldnt EnqueueNDRangeKernel", err);
	}
	kernelEvent.wait();

	std::cout << "Reading back result..." << std::endl;
	auto out_data = out_buffer->ReadData();
	bool vectors_match = true;
	for (int i=0; i<in_data.size(); i++) {
		if (in_data[i] != out_data[0]) {
			std::cout << "Mismatch at element " << i << ": " << in_data[i] << "/" << out_data[i] << std::endl;
			vectors_match = false;
		}
	}
	if (vectors_match) {
		std::cout << "Kernel passthrough test successful" << std::endl;
	}

	//manager->ReleaseBuffer(buffer);
	//manager->ReleaseBuffer(out_buffer);
	delete in_buffer;
	delete out_buffer;
}