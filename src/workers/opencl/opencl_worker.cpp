#include "opencl_worker.h"
#include <iostream>

OpenCLWorker::OpenCLWorker(Component* component, std::string inst_name, int platform_id)
    : Worker(component, __func__, inst_name) {

    manager_ = OpenCLManager::GetReferenceByPlatform(platform_id, this);
    if (manager_ == NULL) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
            "Unable to obtain a reference to an OpenCLManager");
        return;
    } else {
        is_valid_ = true;
    }
}

OpenCLWorker::~OpenCLWorker() {
    
}

bool OpenCLWorker::IsValid() const {
    return is_valid_;
}

bool OpenCLWorker::RunParallel(int num_threads, long long ops_per_thread) {
    bool success;

    if (parallel_kernel_ == NULL) {
        std::string source_file_path(__FILE__);
        auto source_dir = source_file_path.substr(0, source_file_path.find_last_of("/\\")+1);
        std::string filename = source_dir + "parallelthreads.cl";
        
        parallel_kernel_ = InitKernel(*manager_, "runParallel", filename);
        if (parallel_kernel_ == NULL) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "Failed to initialise RunParallel kernel, skipping");
                return false;
        }
    }

    success = parallel_kernel_->SetArgs(ops_per_thread);
    if (!success) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to set args for RunParallel, skipping");
        return false;
    }

    success = parallel_kernel_->Run(0, true, cl::NullRange, cl::NDRange(num_threads), cl::NullRange);
    if (!success) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to execute RunParallel kernel");
        return false;
    }
    
    return true;
}

bool OpenCLWorker::MatrixMult(const std::vector<float>& matA, const std::vector<float>& matB, std::vector<float>& matC) {
    auto bufferA = manager_->CreateBuffer<float>(matA, this);
    auto bufferB = manager_->CreateBuffer<float>(matB, this);
    auto result_buffer = manager_->CreateBuffer<float>(matC.size(), this);

    bool success = MatrixMult(*bufferA, *bufferB, *result_buffer);
    matC = result_buffer->ReadData(true, this);

    manager_->ReleaseBuffer(bufferA, this);
    manager_->ReleaseBuffer(bufferB, this);
    manager_->ReleaseBuffer(result_buffer, this);

    return success;
}

bool OpenCLWorker::MatrixMult(const OCLBuffer<float>& matA, const OCLBuffer<float>& matB, OCLBuffer<float>& matC) {
    cl_uint lenA = (cl_uint)matA.GetSize();
    cl_uint lenB = (cl_uint)matB.GetSize();
    cl_uint lenC = (cl_uint)matC.GetSize();
    
    // Determine the dimensions of the matrices from the lengths of the data
    if (lenA == 0 || lenB == 0 || lenC == 0) {
        if (lenA + lenB + lenC == 0) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "OpenCLWorker::MatrixMult(): warning: performing multiplication on empty matrices");
            return false;
        } else {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "Error during matrix multiplication; sizes of matrices don't match (one had length of 0), "
                "skipping calculation");
            return false;
        }
    }

    cl_ulong Ksquared = ((cl_ulong)lenA*(cl_ulong)lenB)/lenC;
    cl_uint K = (cl_uint)sqrt((/*long*/ double)Ksquared);
    cl_uint M = lenA/K;
    cl_uint N = lenB/K;
    if ((cl_ulong)K*K != Ksquared || (cl_ulong)M*K != lenA || (cl_ulong)N*K != lenB) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Error during matrix multiplication; sizes of matrices don't match, skipping calculation");
        return false;
    }

    if (matrix_kernel_ == NULL) {
        std::string source_file_path(__FILE__);
        auto source_dir = source_file_path.substr(0, source_file_path.find_last_of("/\\")+1);
        std::string filename = source_dir + "matrixmult.cl";

        matrix_kernel_ = InitKernel(*manager_, "matrixMultFull", filename);
        if (matrix_kernel_ == NULL) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "Failed to initialise matrixmult kernel, skipping calculation");
                return false;
        }
    }

    // Specifies the width and height of the blocks used to increase cache performance, will be hardware dependant
    unsigned int block_length = 4;
    cl::Device dev = manager_->GetContext().getInfo<CL_CONTEXT_DEVICES>()[0];
    cl::size_type workgroup_size = matrix_kernel_->GetBackingRef().getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(dev);
    block_length = (unsigned int) sqrt(workgroup_size);
    std::cout << "Block length: " << block_length << std::endl;


    size_t block_data_size = block_length*block_length*sizeof(cl_float);

    unsigned int max_mat_width = std::max(M, K);
    unsigned int max_mat_height = std::max(N, K);

    unsigned int global_width = 0;
	while (global_width < max_mat_width) global_width += block_length;
	unsigned int global_height = 0;
	while (global_height < max_mat_height) global_height += block_length;

    // TODO: Mutex this stuff
    matrix_kernel_->SetArgs(matA, matB, matC, M, K, N,
        cl::Local(block_data_size), cl::Local(block_data_size));

    matrix_kernel_->Run(0, true, cl::NullRange,
        cl::NDRange(global_width, global_height), cl::NDRange(block_length, block_length));

    return true;
}

OpenCLKernel* OpenCLWorker::InitKernel(OpenCLManager& manager, std::string kernel_name, std::string source_file) {
    std::vector<std::string> filenames = {source_file};

    auto kernel_vec = manager_->CreateKernels(filenames, this);
    
    bool found_kernel = false;
    for (auto& kernel : kernel_vec) {
        if (kernel.GetName() == kernel_name) {
            found_kernel = true;
            return new OpenCLKernel(kernel);
        }
    }

    //if (!found_kernel) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
            "Unable to find a kernel called "+kernel_name+" in "+source_file);
        return NULL;
    //}
}