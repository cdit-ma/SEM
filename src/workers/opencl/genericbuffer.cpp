

#include "genericbuffer.h"
#include "openclutilities.h"
#include "OpenCLManager.h"

/*GenericBuffer::GenericBuffer(int id) : id_(id) {
    id_ = OpenCLManager::BufferAttorney::GetNewBufferID(manager)
}*/

GenericBuffer::GenericBuffer(OpenCLManager* manager, size_t size, Worker* worker_ref) :
    manager_(manager), size_(size), worker_reference_(worker_ref) {
    id_ = OpenCLManager::BufferAttorney::GetNewBufferID(*manager);
    
    cl_int err;

    std::cout << "size of new buffer: " << size_ << std::endl;

    buffer_ = std::unique_ptr<cl::Buffer>(new cl::Buffer(manager_->GetContext(), CL_MEM_READ_WRITE, size_, NULL, &err));
    if (err != CL_SUCCESS) {
		LogError(worker_reference_,
			std::string(__func__),
			"Failed to create a buffer",
            err);
        valid_ = false;
		return;
	}

    float* dest = new float[size_/sizeof(float)];
    err = manager_->GetQueues().at(0).enqueueReadBuffer(GetBackingRef(), true, 0, size, dest);
    if(err != CL_SUCCESS){
        LogError(worker_ref,
            __func__,
            "An error occurred while copying data from an OpenCL buffer",
            err);
            return;
    }

    for (int i=0; i<size_/sizeof(float); i++) {
        std::cout << "uninitialised buffer at index " << i << ": " << dest[i] <<std::endl;
    }

    valid_ = true;
}


bool GenericBuffer::is_valid() const{
    return valid_;
}

size_t GenericBuffer::GetSize() const {
    return size_;
}

int GenericBuffer::GetID() const {
    return id_;
}


const cl::Buffer& GenericBuffer::GetBackingRef() const {
    return *(buffer_.get());
}


bool GenericBuffer::ReadData(void* dest, size_t size, bool blocking, Worker* worker_reference) {
    cl_int err;

    if (size < 40) {
        float* data = (float*) dest;
        for (int i=0; i<(size/sizeof(float)); i++) {
            std::cout << data[i] << std::endl;
        }
    }

    // NEEDS TO HAVE LENGTH INITIALISED FIRST
    //err = cl::copy(manager_->GetQueues().at(0), buffer_, data.begin(), data.end());
    std::cout << &(GetBackingRef()) << std::endl;
    err = manager_->GetQueues().at(0).enqueueReadBuffer(GetBackingRef(), blocking, 0, size, dest);
    if(err != CL_SUCCESS){
        LogError(worker_reference,
            __func__,
            "An error occurred while copying data from an OpenCL buffer",
            err);
            return false;
    }

    if (size < 40) {
        float* data = (float*) dest;
        for (int i=0; i<(size/sizeof(float)); i++) {
            std::cout << data[i] << std::endl;
        }
    }

    return true;
}

bool GenericBuffer::WriteData(const void* source, size_t size, bool blocking, Worker* worker_reference) {
    cl_int err;

    if (size < 40) {
        float* data = (float*) source;
        for (int i=0; i<(size/sizeof(float)); i++) {
            std::cout << data[i] << std::endl;
        }
    }

    // TODO: allow for non-blocking variant
    //err = cl::copy(manager_->GetQueues().at(0), data.begin(), data.end(), buffer_);
    std::cout << &(GetBackingRef()) << std::endl;
    err = manager_->GetQueues().at(0).enqueueWriteBuffer(GetBackingRef(), blocking, 0, size, source);
    if(err != CL_SUCCESS){
        LogError(worker_reference,
            __func__,
            "An error occurred while copying data to an OpenCL buffer",
            err);
            return false;
    }


    std::cout << &(GetBackingRef()) << std::endl;
    float* dest = new float[size_/sizeof(float)];
    err = manager_->GetQueues().at(0).enqueueReadBuffer(GetBackingRef(), true, 0, size, dest);
    if(err != CL_SUCCESS){
        LogError(worker_reference,
            __func__,
            "An error occurred while copying data from an OpenCL buffer",
            err);
            return false;
    }

    for (int i=0; i<size_/sizeof(float); i++) {
        std::cout << "after write at index " << i << ": " << dest[i] <<std::endl;
    }


    return true;
}

void GenericBuffer::LogError(Worker* worker_reference, std::string function_name, std::string error_message,
    int cl_error_code)
{
    LogOpenCLError(worker_reference,
        "OCLBuffer::" + function_name,
        error_message,
        cl_error_code);
}

void GenericBuffer::LogError(Worker* worker_reference, std::string function_name, std::string error_message)
{
    LogOpenCLError(worker_reference,
        "OCLBuffer::" + function_name,
        error_message);
}