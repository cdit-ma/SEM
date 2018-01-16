#include "opencldevice.h"

#include "openclmanager.h"
#include "openclutilities.h"


/*std::vector<cl::Device> device_store;

cl::Device& storeDevice(cl::Device& dev) {
	device_store.reserve(10);
	cl::Device new_device(dev);
	device_store.push_back(new_device);
	//std::cout << & device_store.back() << " vs " << &dev << std::endl;
	return device_store.back();
	//return *new  cl::Device(dev);
}*/


OpenCLDevice::OpenCLDevice(const Worker& worker, OpenCLManager& manager, cl::Device& device) :
    dev_(new cl::Device(device)),
    manager_(manager),
    queue_(new cl::CommandQueue(manager_.GetContext(), *dev_, CL_QUEUE_PROFILING_ENABLE, &err_)),
    name_(dev_->getInfo<CL_DEVICE_NAME>())
{
    // /int err;
    //auto command_queue = new cl::CommandQueue(ctx, device, CL_QUEUE_PROFILING_ENABLE, &err);

    if (err_ != CL_SUCCESS) {
        LogError(worker,
            __func__,
            "Unable to create a command queue for device " + name_);
    }
	//name_ = dev_.getInfo<CL_DEVICE_NAME>();
    valid_ = true;
}

const cl::Device& OpenCLDevice::GetRef() const {
    return *dev_;
}

std::string OpenCLDevice::GetName() const {
    return name_;
}

const cl::CommandQueue& OpenCLDevice::GetQueue() const {
    return *queue_;
}

// May throw
bool OpenCLDevice::LoadKernelsFromSource(const Worker& worker, const std::vector<std::string>& filenames) {
    cl_int err;

	std::vector<OpenCLKernel> kernels;

	// Read, compile and link the Program from OpenCL code
	cl::Program::Sources sources = ReadOpenCLSourceCode(filenames);

	programs_.emplace_back(manager_.GetContext(), sources, &err);
	if (err != CL_SUCCESS) {
		LogError(worker,
			std::string(__func__),
			"Unable to create OpenCL program from OpenCL source code",
			err);
		return false;
	}
    cl::Program& new_program = programs_.back();

    std::vector<cl::Device> device_vec;
    device_vec.emplace_back(*dev_);
	err = new_program.build(device_vec);
	if (err != CL_SUCCESS) {
		LogError(worker,
			std::string(__func__),
			"An error occurred while building an OpenCL program: \n" +
			new_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(*dev_),
			err);
		return false;
	}

    std::vector<cl::Kernel> new_kernels;
	err = new_program.createKernels(&new_kernels);
	if (err != CL_SUCCESS) {
		LogError(worker,
			std::string(__func__),
			"An error occurred during the creation of OpenCL kernels from a built program",
			err);
		return false;
	}

	for (auto& kernel : new_kernels) {
		kernels_.emplace_back(worker, manager_, kernel);
	}

	return true;
}

// May throw
bool OpenCLDevice::LoadKernelsFromBinary(const Worker& worker, const std::string& filename) {
	cl_int err;

	std::vector<OpenCLKernel> kernels;

	std::vector<std::string> filenames;
	filenames.push_back(filename);
	cl::Program::Binaries binaries = ReadOpenCLBinaries(filenames);

    std::vector<cl::Device> device_vec;
    device_vec.push_back(*dev_);
	std::vector<cl_int> binary_success;

	programs_.emplace_back(manager_.GetContext(), device_vec, binaries, &binary_success, &err);
	if (err != CL_SUCCESS) {
		LogError(worker,
			std::string(__func__),
			"Unable to create OpenCL program from OpenCL binary",
			err);
		return false;
	}
	for (auto& result : binary_success) {
		if (result != CL_SUCCESS) {
			LogError(worker,
				std::string(__func__),
				"An error occurred while loading an OpenCL binary",
				err);
		return false;
		}
	}

    cl::Program& new_program = programs_.back();
	err = new_program.build(device_vec);
	if (err != CL_SUCCESS) {
		LogError(worker,
			std::string(__func__),
			"An error occurred while building an OpenCL program: \n" +
				new_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(*dev_),
				err);
		return false;
	}

    std::vector<cl::Kernel> new_kernels;
	err = new_program.createKernels(&new_kernels);
	if (err != CL_SUCCESS) {
		LogError(worker,
			std::string(__func__),
			"An error occurred during the creation of OpenCL kernels from a built program",
			err);
		return false;
	}

	for (auto& kernel : new_kernels) {
		std::string name = kernel.getInfo<CL_KERNEL_FUNCTION_NAME>();
		bool name_already_exists = false;
		for (const auto& k : kernels_) {
			if (k.GetName() == name) {
				name_already_exists = true;
			}
		}
		if (name_already_exists) {
			LogError(worker,
				std::string(__func__),
				"Attempting to add kernel named "+name+" when a kernel with that name already exists, skipping",
				err);
			continue;
		}
		kernels_.emplace_back(worker, manager_, kernel);
		//std::cout << name << std::endl;
	}

	return true;
}

const std::vector<std::reference_wrapper<OpenCLKernel> > OpenCLDevice::GetKernels() {
	std::vector<std::reference_wrapper<OpenCLKernel> > kernel_refs;
	for (OpenCLKernel& kernel : kernels_) {
		kernel_refs.emplace_back(std::ref(kernel));
	}
	return kernel_refs;
}

/*OpenCLKernel& OpenCLDevice::GetKernel(const std::string& kernel_name) const {
	for (const auto& kernel : kernels_) {
		if (kernel.GetName() == kernel_name) {
			return kernel;
		}
	}
}*/

void OpenCLDevice::LogError(const Worker& worker, std::string function_name, std::string error_message, cl_int cl_error_code) {
    LogOpenCLError(worker,
        "OpenCLKernelBase::" + function_name,
        error_message,
        cl_error_code);
}

void OpenCLDevice::LogError(const Worker& worker, std::string function_name, std::string error_message) {
    LogOpenCLError(worker,
        "OpenCLKernelBase::" + function_name,
        error_message);
}