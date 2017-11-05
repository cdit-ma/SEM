

#include "genericbuffer.h"
#include "openclutilities.h"
#include "openclmanager.h"

/*GenericBuffer::GenericBuffer(int id) : id_(id) {
    id_ = OpenCLManager::BufferAttorney::GetNewBufferID(manager)
}*/

GenericBuffer::GenericBuffer(OpenCLManager& manager, size_t size, Worker* worker_ref) :
    manager_(manager), size_(size), worker_reference_(worker_ref) {
    id_ = OpenCLManager::BufferAttorney::GetNewBufferID(manager, *this);
    if (id_ == OpenCLManager::invalid_buffer_id_) {
        LogError(worker_reference_,
			std::string(__func__),
			"Failed to obtain tracking ID for buffer from OpenCLManager");
        valid_ = false;
        return;
    }
    
    cl_int err;

    buffer_ = std::unique_ptr<cl::Buffer>(new cl::Buffer(manager_.GetContext(), CL_MEM_READ_WRITE, size_, NULL, &err));
    if (err != CL_SUCCESS) {
		LogError(worker_reference_,
			std::string(__func__),
			"Failed to create a buffer",
            err);
        valid_ = false;
		return;
	}

    valid_ = true;
}

GenericBuffer::~GenericBuffer() {
    if (valid_) {
        valid_ = false;
        OpenCLManager::BufferAttorney::ReleaseBufferID(manager_, *this);
    }
}

static_assert(std::is_nothrow_move_constructible<GenericBuffer>::value, "MyType should be noexcept MoveConstructible");


bool GenericBuffer::is_valid() const{
    return valid_;
}

size_t GenericBuffer::GetSize() const {
    return size_;
}

int GenericBuffer::GetID() const {
    return id_;
}

Worker* GenericBuffer::GetInitialWorker() const {
    return worker_reference_;
}

const cl::Buffer& GenericBuffer::GetBackingRef() const {
    return *(buffer_.get());
}


bool GenericBuffer::ReadData(void* dest, size_t size, bool blocking, Worker* worker_reference) const {
    cl_int err;

    // NEEDS TO HAVE LENGTH INITIALISED FIRST
    //err = cl::copy(manager_->GetQueues().at(0), buffer_, data.begin(), data.end());
    err = manager_.GetQueues().at(0)->enqueueReadBuffer(GetBackingRef(), blocking, 0, size, dest);
    if(err != CL_SUCCESS){
        LogError(worker_reference,
            __func__,
            "An error occurred while copying data from an OpenCL buffer",
            err);
            return false;
    }

    return true;
}

bool GenericBuffer::WriteData(const void* source, size_t size, bool blocking, Worker* worker_reference) {
    cl_int err;

    //err = cl::copy(manager_->GetQueues().at(0), data.begin(), data.end(), buffer_);
    err = manager_.GetQueues().at(0)->enqueueWriteBuffer(GetBackingRef(), blocking, 0, size, source);
    if(err != CL_SUCCESS){
        LogError(worker_reference,
            __func__,
            "An error occurred while copying data to an OpenCL buffer",
            err);
            return false;
    }

    return true;
}

void GenericBuffer::LogError(Worker* worker_reference, std::string function_name, std::string error_message,
    int cl_error_code) const
{
    LogOpenCLError(worker_reference,
        "GenericBuffer::" + function_name,
        error_message,
        cl_error_code);
}

void GenericBuffer::LogError(Worker* worker_reference, std::string function_name, std::string error_message) const
{
    LogOpenCLError(worker_reference,
        "GenericBuffer::" + function_name,
        error_message);
}