

#include <iostream>
#include <vector>

#include "openclmanager.h"
#include "rekernel.hpp"

void printFail(std::string message) {
	std::cerr << "\033[31m" << message << "\033[0m" <<std::endl;
}

int main(int argc, char** argv) {
	
	cl_int err;

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
	
	// Perform basic initialisation task with the first platform found
	std::cout << "Creating OpenCLManager for platform 0 12" << std::endl;
	auto manager = OpenCLManager::GetReferenceByPlatform(0);
	if (manager->IsValid()) {
		std::cout << "Successfully created OpenCL manager" << std::endl;
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
	// For now, just add the "kernel" OpenCL file in the source directory
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
			std::cout << " - " << k.getInfo<CL_KERNEL_FUNCTION_NAME>() << std::endl;
		}
	}

	// Create a test buffer
	auto buffer = manager->CreateBuffer<float>(4);
	//OCLBuffer<std::string>* stringBuffer = manager->CreateBuffer<std::string>(1024);

	// Send some data through
	std::cout << "Writing data to a buffer (4 elements containing 0.5f)..." << std::endl;
	buffer->WriteData(std::vector<float>(4, 0.5f));

	// Read it back
	std::cout << "Reading the data back..." << std::endl;
	auto read_buffer = buffer->ReadData();
	for (auto e : read_buffer) {
		std::cout << e << std::endl;
	}

	delete buffer;

	auto in_buffer = manager->CreateBuffer<float>(std::vector<float>(4, 0.5f));
	auto out_buffer = manager->CreateBuffer<float>(4);

	cl::Kernel pt_kernel;
	bool found_pt_kernel = false;
	for (auto& k : kernels) {
		if (k.getInfo<CL_KERNEL_FUNCTION_NAME>() == "dataPassthroughTest") {
			pt_kernel = k;
			found_pt_kernel = true;
		}
	}
	if (!found_pt_kernel) {
		printFail("Unable to find dataPasshtoughTest kernel");
	}

	/*err = pt_kernel.setArg(0, (buffer->get_backing_ref()));
	if (err != CL_SUCCESS) {
		LogOpenCLError(NULL, __func__,
			"Couldnt set input buffer kernel arg", err);
	}
	std::cout << sizeof(buffer->get_backing_ref()) <<std::endl;
	err = pt_kernel.setArg(1, (out_buffer->get_backing_ref()));
	//err = pt_kernel.setArg(1, manager->CreateBuffer<float>(2)->get_backing_ref());
	//err = pt_kernel.setArg(1, 2);
	if (err != CL_SUCCESS) {
		LogOpenCLError(NULL, __func__,
			"Couldnt set output buffer kernel arg", err);
	}*/
	
	ReKernel kernel(*manager, pt_kernel, NULL);
	//kernel.SetArgs(2, out_buffer->get_backing_ref());
	//::clSetKernelArg(pt_kernel.get(), 1, sizeof(int), (int)2);
	//kernel.SetArgs(1, 0.5f, '4', '4');
	kernel.SetArgs(in_buffer->get_backing_ref(), out_buffer->get_backing_ref());

	cl::Event kernelEvent;
	err = manager->GetQueues()[0].enqueueNDRangeKernel(pt_kernel, 
												cl::NullRange, cl::NDRange(4), cl::NDRange(4),
												NULL, &kernelEvent);
	if (err != CL_SUCCESS) {
		LogOpenCLError(NULL, __func__, "Couldnt EnqueueNDRangeKernel", err);
	}
	kernelEvent.wait();

	read_buffer = out_buffer->ReadData();
	for (auto e : read_buffer) {
		std::cout << e << std::endl;
	}

	//manager->ReleaseBuffer(buffer);
	//manager->ReleaseBuffer(out_buffer);
	delete in_buffer;
	delete out_buffer;
	
	std::cout << "Finished" << std::endl;

	system("pause");
}