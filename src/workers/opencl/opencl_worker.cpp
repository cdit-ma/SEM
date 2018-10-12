#include "opencl_worker.h"
#include "openclutilities.h"

#include <math.h>
#include <iostream>

#include <exception>

OpenCL_Worker::OpenCL_Worker(const BehaviourContainer& bc, std::string inst_name)
    : Worker(bc, __func__, inst_name) {

    platform_id_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::INTEGER, "platform_id").lock();
    if (platform_id_ != NULL) {
        platform_id_->set_Integer(-1);
    }

    device_id_ = Activatable::ConstructAttribute(ATTRIBUTE_TYPE::INTEGER, "device_id").lock();
    if (device_id_ != NULL) {
        device_id_->set_Integer(-1);
    }
}

void OpenCL_Worker::HandleConfigure() {
    int platform_id;
    int device_id;
    auto platform_attr = GetAttribute("platform_id").lock();
    if (platform_attr != NULL) {
        platform_id = platform_attr->get_Integer();
    }
    auto device_attr = GetAttribute("device_id").lock();
    if (device_attr != NULL) {
        device_id = device_attr->get_Integer();
    }

    manager_ = OpenCLManager::GetReferenceByPlatformID(*this, platform_id);
    if (manager_ == NULL) {
        throw std::runtime_error("Unable to obtain a reference to an OpenCLManager");
    }

    auto& device_list = manager_->GetDevices(*this);
    std::vector<unsigned int> device_ids;

    if (device_id < -1 || device_id >= device_list.size()) {
        throw std::runtime_error("'device_id' provided by attribute is out of bounds: " + std::to_string(device_id));
    }

    if (device_id == -1) {
        for (int i=0; i<(int)device_list.size(); i++) {
            devices_.emplace_back(std::ref(*device_list.at(i)));
            device_ids.push_back(i);
        }
    } else {
        devices_.emplace_back(std::ref(*device_list.at(device_id)));
        device_ids.push_back(device_id);
    }

    load_balancer_ = new OpenCLLoadBalancer(device_ids);

    InitFFT();

    is_valid_ = true;

    Worker::HandleConfigure();
}

void OpenCL_Worker::HandleTerminate() {
    if (!CleanupFFT()) {
        throw std::runtime_error("Cannot clean up FFT");
    }
    delete load_balancer_;
}

bool OpenCL_Worker::IsValid() const {
    return is_valid_;
}

bool OpenCL_Worker::RunParallel(int num_threads, long long ops_per_thread) {
    bool success = false;

    auto device_id = load_balancer_->RequestDevice();
    auto& device = manager_->GetDevices(*this)[device_id];

    std::string filename = GetSourcePath("parallelthreads.cl");
    try {
        auto& parallel_kernel = GetKernel(*device, "runParallel", filename);
        auto kernel_lock = parallel_kernel.AcquireLock();

        parallel_kernel.SetArgs(ops_per_thread);

        parallel_kernel.Run(*device, true, cl::NullRange, cl::NDRange(num_threads), cl::NullRange,
            std::move(kernel_lock));

        success = true;
    } catch (const std::exception& e) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to execute RunParallel kernel: \n"+std::string(e.what()));
    }

    load_balancer_->ReleaseDevice(device_id);
    
    return success;
}

bool OpenCL_Worker::MatrixMult(const std::vector<float>& matA, const std::vector<float>& matB, std::vector<float>& matC) {
    auto device_id = load_balancer_->RequestDevice();
    auto& device = manager_->GetDevices(*this)[device_id];

    OpenCLBuffer<float> bufferA;
    OpenCLBuffer<float> bufferB;
    OpenCLBuffer<float> result_buffer;

    try {
        bufferA = manager_->CreateBuffer<float>(*this, matA, *device);
    } catch (const std::exception& e) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to create buffer for Matrix A in MatrixMult: \n"+std::string(e.what()));
        return false;
    }

    try {
        bufferB = manager_->CreateBuffer<float>(*this, matB, *device);
    } catch (const std::exception& e) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to create buffer for Matrix B in MatrixMult: \n"+std::string(e.what()));
        return false;
    }
    
    try {
        result_buffer = manager_->CreateBuffer<float>(*this, matC, *device);
    } catch (const std::exception& e) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to create buffer for result matrix in MatrixMult: \n"+std::string(e.what()));
        return false;
    }

    bool success = MatrixMult(bufferA, bufferB, result_buffer, *device);
    matC = result_buffer.ReadData(*this, *device, true);

    load_balancer_->ReleaseDevice(device_id);

    return success;
}

bool OpenCL_Worker::MatrixMult(const OpenCLBuffer<float>& matA, const OpenCLBuffer<float>& matB, OpenCLBuffer<float>& matC, OpenCLDevice& device) {
    cl_uint lenA = (cl_uint)matA.GetNumElements();
    cl_uint lenB = (cl_uint)matB.GetNumElements();
    cl_uint lenC = (cl_uint)matC.GetNumElements();

    // Determine the dimensions of the matrices from the lengths of the data
    if (lenA == 0 || lenB == 0 || lenC == 0) {
        if (lenA + lenB + lenC == 0) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "OpenCL_Worker::MatrixMult(): warning: performing multiplication on empty matrices");
            return false;
        } else {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "Error during matrix multiplication; sizes of matrices don't match (one had length of 0), "
                "skipping calculation");
            return false;
        }
    }

    cl_ulong Ksquared = ((cl_ulong)lenA*(cl_ulong)lenB)/lenC;
    cl_uint K = (cl_uint)sqrt((double)Ksquared);
    cl_uint M = lenA/K;
    cl_uint N = lenB/K;
    if ((cl_ulong)K*K != Ksquared || (cl_ulong)M*K != lenA || (cl_ulong)N*K != lenB) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Error during matrix multiplication; sizes of matrices don't match, skipping calculation");
        return false;
    }

    std::string filename = GetSourcePath("matrixmult.cl");
    auto& matrix_kernel = GetKernel(device, "matrixMultFull", filename);

    // Specifies the width and height of the blocks used to increase cache performance, will be hardware dependant
    unsigned int block_length;
    cl::size_type kernel_workgroup_size = matrix_kernel.GetBackingRef().getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device.GetRef());
    cl::size_type device_workgroup_size = device.GetRef().getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
    auto workgroup_size = std::min(kernel_workgroup_size, device_workgroup_size);
    block_length = (unsigned int) sqrt(workgroup_size);

    cl::LocalSpaceArg block_data_size = cl::Local(block_length*block_length*sizeof(cl_float));

    unsigned int max_mat_width = std::max(M, K);
    unsigned int max_mat_height = std::max(N, K);

    unsigned int global_width = 0;
	while (global_width < max_mat_width) global_width += block_length;
	unsigned int global_height = 0;
	while (global_height < max_mat_height) global_height += block_length;

    {
        std::unique_lock<std::mutex> kernel_lock(matrix_kernel.AcquireLock());

        try {
            matrix_kernel.SetArgs(matA, matB, matC, M, K, N, block_data_size, block_data_size);
        } catch (const OpenCLException& ocle) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "Failed to set args for MatrixMult:\n"+std::string(ocle.what()));
                return false;
        }
        
        try {
            matrix_kernel.Run(device, true, cl::NullRange,
                cl::NDRange(global_width, global_height), cl::NDRange(block_length, block_length),
                std::move(kernel_lock));
        } catch (const OpenCLException& ocle) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "Failed to successfully run MatrixMult kernel"+std::string(ocle.what()));
                return false;
        }
    }

    return true;
}


bool OpenCL_Worker::KmeansCluster(const std::vector<float>& points, std::vector<float>& centroids, std::vector<int>& point_classifications, int iterations) {
    if (manager_->IsFPGA()) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
                "KMeans implementation not provided for FPGA platforms");
        return false;
    }

    auto device_id = load_balancer_->RequestDevice();
    auto& device = manager_->GetDevices(*this)[device_id];

    OpenCLBuffer<float> point_buffer;
    OpenCLBuffer<float> centroid_buffer;
    OpenCLBuffer<int> classification_buffer;

    try {
        point_buffer = manager_->CreateBuffer(*this, points, *device);
    } catch (const std::exception& e) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to create buffer for points in KMeans: \n"+std::string(e.what()));
        return false;
    }

    try {
        centroid_buffer = manager_->CreateBuffer(*this, centroids, *device);
    } catch (const std::exception& e) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to create buffer for point classifications in KMeans: \n"+std::string(e.what()));
        return false;
    }

    try {
        classification_buffer = manager_->CreateBuffer(*this, point_classifications, *device);
    } catch (const std::exception& e) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to create buffer for point classifications in KMeans: \n"+std::string(e.what()));
        return false;
    }

    bool success = KmeansCluster(point_buffer, centroid_buffer, classification_buffer, iterations);
    if (!success) {
        return false;
    }

    centroids = ReadBuffer(centroid_buffer);
    point_classifications = ReadBuffer(classification_buffer, true);
    return true;
}

bool OpenCL_Worker::KmeansCluster(const OpenCLBuffer<float>& points, OpenCLBuffer<float>& centroids, OpenCLBuffer<int>& point_classifications, int iterations) {
    
    auto device_id = load_balancer_->RequestDevice();
    const auto& device = manager_->GetDevices(*this)[device_id];    

    std::string filename = GetSourcePath("kmeans.cl");
    auto& cluster_classify_kernel = GetKernel(*device, "classifyPoints", filename);
    auto& cluster_adjust_kernel = GetKernel(*device, "adjustCentroids", filename);

    cl_uint num_centroids = (cl_uint) centroids.GetNumElements() / (sizeof(cl_float4) / sizeof(float));
    cl_uint num_points = (cl_uint) points.GetNumElements() / (sizeof(cl_float4) / sizeof(float));
    cl_uint num_classes = (cl_uint) point_classifications.GetNumElements();
    if (num_points != num_classes) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Attempting to run K-means cluster algorithm, but the point and classification vector lengths don't match");
        return false;
    }

    // Calculate classification parameters
    cl::NDRange global_classify_threads(num_points);
    std::unique_lock<std::mutex> classify_kernel_lock(cluster_classify_kernel.AcquireLock());
    try {
        cluster_classify_kernel.SetArgs(points, point_classifications, centroids, num_centroids);
    } catch (const OpenCLException& ocle) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to set args for cluster classify kernel, skipping:\n"+std::string(ocle.what()));
        return false;
    }
    
    // Calculate adjust parameters
    cl_uint num_compute_units = device->GetRef().getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
    auto kernel_wg_size = cluster_adjust_kernel.GetBackingRef().getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device->GetRef());
    auto device_wg_size = device->GetRef().getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
    // need to ensure that theres space for both the local centroid and count buffers
    cl_ulong mem_wg_size = device->GetRef().getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() / (sizeof(cl_float4)+sizeof(cl_uint)) / num_compute_units;

    auto adjust_local_size = kernel_wg_size < device_wg_size ? kernel_wg_size : device_wg_size;
    adjust_local_size = adjust_local_size < mem_wg_size ? adjust_local_size : mem_wg_size;

    cl::NDRange adjust_local_range(adjust_local_size);
    cl::NDRange adjust_global_range(adjust_local_size*num_compute_units);

    cl::LocalSpaceArg local_center_buffer = cl::Local(adjust_local_size*num_centroids*sizeof(cl_float4));
    cl::LocalSpaceArg local_count_buffer = cl::Local(adjust_local_size*num_centroids*sizeof(cl_uint));

    OpenCLBuffer<float> work_group_center_buffer;
    OpenCLBuffer<cl_uint> work_group_count_buffer;

    try {
        work_group_center_buffer = manager_->CreateBuffer<float>(*this, num_centroids*num_compute_units*4);
    } catch (const std::exception& e) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to create buffer for work_group_center_buffer in KMeans: \n"+std::string(e.what()));
        return false;
    }

    try {
        work_group_count_buffer = manager_->CreateBuffer<cl_uint>(*this, num_centroids*num_compute_units);
    } catch (const std::exception& e) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to create buffer for work_group_center_buffer in KMeans: \n"+std::string(e.what()));
        return false;
    }

    std::unique_lock<std::mutex> adjust_kernel_lock(cluster_adjust_kernel.AcquireLock());
    try {
        cluster_adjust_kernel.SetArgs(points, point_classifications, num_points, centroids, num_centroids,
            local_center_buffer, local_count_buffer, work_group_center_buffer, work_group_count_buffer);
    } catch (const OpenCLException& ocle) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to set args for cluster adjust kernel, skipping:\n"+std::string(ocle.what()));
        return false;
    }
    
    for (int iter=0; iter < iterations; iter++) {
        // Identify the nearest centroid for each point
        try {
            cluster_classify_kernel.Run(*device, true, cl::NullRange, global_classify_threads, cl::NullRange);
        } catch (const OpenCLException& ocle) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "Failed to execute K-means classification kernel:\n"+std::string(ocle.what()));
            return false;
        }

        // Find the averaged position of all the points associated with each given centroid
        try {
            cluster_adjust_kernel.Run(*device, true, cl::NullRange, adjust_global_range, adjust_local_range);
        } catch (const OpenCLException& ocle) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "Failed to execute K-means center adjustment kernel:\n"+std::string(ocle.what()));
            return false;
        }

        //Read back the per-work-group result from the adjust kernel
        auto wg_center_vec = work_group_center_buffer.ReadData(*this, *device, true);
        auto wg_count_vec = work_group_count_buffer.ReadData(*this, *device, true);
        if (wg_center_vec.size() != wg_count_vec.size()*4) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "Error reading back adjusted centroids aggregated per workgroup; center and count vector size mismatch");
            return false;
        }
         
        // Perform the final part of the reduction
        std::vector<float> new_centroids;
        float center_accumulator[4] = {0,0,0,0};
        cl_uint count_accumulator = 0;
        unsigned int centers_processed = 0;
        for (unsigned int i=0; i<wg_count_vec.size(); i+=1) {
            center_accumulator[0] += wg_center_vec[i*4+0];
            center_accumulator[1] += wg_center_vec[i*4+1];
            center_accumulator[2] += wg_center_vec[i*4+2];
            center_accumulator[3] += wg_center_vec[i*4+3];
            count_accumulator += wg_count_vec[i];
            centers_processed++;

            if (centers_processed >= num_compute_units) {
                if (count_accumulator > 0){
                    new_centroids.push_back(center_accumulator[0] / count_accumulator);
                    new_centroids.push_back(center_accumulator[1] / count_accumulator);
                    new_centroids.push_back(center_accumulator[2] / count_accumulator);
                    new_centroids.push_back(center_accumulator[3] / count_accumulator);
                } else {
                    new_centroids.push_back(0);
                    new_centroids.push_back(0);
                    new_centroids.push_back(0);
                    new_centroids.push_back(0);
                }
                center_accumulator[0] = 0;
                center_accumulator[1] = 0;
                center_accumulator[2] = 0;
                center_accumulator[3] = 0;
                count_accumulator = 0;
                centers_processed = 0;
            } 
        }

        // Update the centers to match
        centroids.WriteData(*this, new_centroids, *device, true);
    }

    classify_kernel_lock.unlock();
    adjust_kernel_lock.unlock();

    load_balancer_->ReleaseDevice(device_id);

    return true;
}

void OpenCL_Worker::Log(std::string function_name, ModelLogger::WorkloadEvent event, int work_id, std::string args) {
    Worker::Log("OpenCL_Worker::"+function_name, event, work_id, args);
    std::cerr << "OpenCL_Worker::" << function_name << ", " << args << std::endl;
}

OpenCLKernel& OpenCL_Worker::GetKernel(OpenCLDevice& device, const std::string& kernel_name, const std::string& source_file) {
    for (OpenCLKernel& kernel : device.GetKernels()) {
        if (kernel.GetName() == kernel_name) {
            return kernel;
        }
    }

    std::vector<std::string> filenames;
    filenames.push_back(source_file);
    device.LoadKernelsFromSource(*this, filenames);

    auto kernel_vec = device.GetKernels();
    if (kernel_vec.size() == 0) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
        "Unable to find any kernels in "+source_file);
        throw std::invalid_argument("No kernels in file "+source_file);
    }
    
    for (OpenCLKernel& kernel : kernel_vec) {
        if (kernel.GetName() == kernel_name) {
            return kernel;
        }
    }

    Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
        "Unable to find a kernel called "+kernel_name+" in "+source_file);
    throw std::invalid_argument("Unable to find kernel with name "+kernel_name+" in file "+source_file);
}
