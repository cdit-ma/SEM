#include "opencl_worker.h"
#include "openclutilities.h"

#include <math.h>
#include <iostream>

#include <exception>

OpenCLWorker::OpenCLWorker(Component* component, std::string inst_name)
    : Worker(component, __func__, inst_name) {
    
}

OpenCLWorker::~OpenCLWorker() {
    
}

bool OpenCLWorker::Configure(int platform_id, int device_id) {
    // TODO: Pull out platforms and devices using GetAttribute later

    manager_ = OpenCLManager::GetReferenceByPlatform(platform_id, this);
    if (manager_ == NULL) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
            "Unable to obtain a reference to an OpenCLManager");
        return false;
    } else {
        is_valid_ = true;
    }

    std::vector<unsigned int> device_ids;
    device_ids.push_back(device_id);
    load_balancer_ = new OpenCLLoadBalancer(device_ids);
    auto& device_list = manager_->GetDevices(this);
    devices_.emplace_back(device_list.at(std::ref(device_id)));

    return true;
}

bool OpenCLWorker::IsValid() const {
    return is_valid_;
}

bool OpenCLWorker::RunParallel(int num_threads, long long ops_per_thread) {
    bool success;

    auto device_id = load_balancer_->RequestDevice();
    auto& device = manager_->GetDevices()[device_id];

    std::string filename = GetSourcePath("parallelthreads.cl");
    auto& parallel_kernel = GetKernel(device, "runParallel", filename);

    success = parallel_kernel.SetArgs(ops_per_thread);
    if (!success) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to set args for RunParallel, skipping");
        return false;
    }

    success = parallel_kernel.Run(device, true, cl::NullRange, cl::NDRange(num_threads), cl::NullRange);
    if (!success) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to execute RunParallel kernel");
        return false;
    }

    load_balancer_->ReleaseDevice(device_id);
    
    return true;
}

bool OpenCLWorker::MatrixMult(const std::vector<float>& matA, const std::vector<float>& matB, std::vector<float>& matC) {
    auto device_id = load_balancer_->RequestDevice();
    auto& device = manager_->GetDevices()[device_id];

    auto bufferA = manager_->CreateBuffer<float>(matA, device, this);
    auto bufferB = manager_->CreateBuffer<float>(matB, device, this);
    //auto result_buffer = manager_->CreateBuffer<float>(matC.size(), this);
    auto result_buffer = manager_->CreateBuffer<float>(matC, device, this);

    bool success = MatrixMult(*bufferA, *bufferB, *result_buffer, device);
    //auto new_matA = bufferA->ReadData(true, this);
    //auto new_matB = bufferB->ReadData(true, this);
    matC = result_buffer->ReadData(device, true, this);

    manager_->ReleaseBuffer(bufferA, this);
    manager_->ReleaseBuffer(bufferB, this);
    manager_->ReleaseBuffer(result_buffer, this);

    load_balancer_->ReleaseDevice(device_id);

    return success;
}

bool OpenCLWorker::MatrixMult(const OCLBuffer<float>& matA, const OCLBuffer<float>& matB, OCLBuffer<float>& matC, OpenCLDevice& device) {
    cl_uint lenA = (cl_uint)matA.GetNumElements();
    cl_uint lenB = (cl_uint)matB.GetNumElements();
    cl_uint lenC = (cl_uint)matC.GetNumElements();

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

    std::string filename = GetSourcePath("matrixmult.cl");
    auto& matrix_kernel = GetKernel(device, "matrixMultFull", filename);

    // Specifies the width and height of the blocks used to increase cache performance, will be hardware dependant
    unsigned int block_length = 4;
    cl::Device dev = manager_->GetContext().getInfo<CL_CONTEXT_DEVICES>()[0];
    cl::size_type workgroup_size = matrix_kernel.GetBackingRef().getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(dev);
    block_length = (unsigned int) sqrt(workgroup_size);


    cl::LocalSpaceArg block_data_size = cl::Local(block_length*block_length*sizeof(cl_float));

    unsigned int max_mat_width = std::max(M, K);
    unsigned int max_mat_height = std::max(N, K);

    unsigned int global_width = 0;
	while (global_width < max_mat_width) global_width += block_length;
	unsigned int global_height = 0;
	while (global_height < max_mat_height) global_height += block_length;

    // TODO: Mutex this stuff
    bool did_set_args = matrix_kernel.SetArgs(matA, matB, matC, M, K, N,
        block_data_size, block_data_size);
    if (!did_set_args) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to set args for MatrixMult");
            return false;
    }

    bool did_kernel_complete = matrix_kernel.Run(device, true, cl::NullRange,
        cl::NDRange(global_width, global_height), cl::NDRange(block_length, block_length));
    if (!did_kernel_complete) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to successfully run MatrixMult kernel");
            return false;
    }

    return true;
}


bool OpenCLWorker::KmeansCluster(const std::vector<float>& points, std::vector<float>& centroids, std::vector<int>& point_classifications, int iterations) {
    auto point_buffer = CreateBuffer(points);
    auto centroid_buffer = CreateBuffer(centroids);
    auto classification_buffer = CreateBuffer(point_classifications, true);

    bool success = KmeansCluster(*point_buffer, *centroid_buffer, *classification_buffer, iterations);
    if (!success) {
        return false;
    }

    centroids = ReadBuffer(*centroid_buffer);
    point_classifications = ReadBuffer(*classification_buffer, true);
    return true;
}

bool OpenCLWorker::KmeansCluster(const OCLBuffer<float>& points, OCLBuffer<float>& centroids, OCLBuffer<int>& point_classifications, int iterations) {
    bool success;

    auto device_id = load_balancer_->RequestDevice();
    auto& device = manager_->GetDevices()[device_id];

    std::string filename = GetSourcePath("kmeans.cl");
    auto& cluster_classify_kernel = GetKernel(device, "classifyPoints", filename);
    auto& cluster_adjust_kernel = GetKernel(device, "adjustCentroids", filename);

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
    success = cluster_classify_kernel.SetArgs(points, point_classifications, centroids, num_centroids);
    if (!success) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to set args for cluster classify kernel, skipping");
        return false;
    }
    
    // Calculate adjust parameters
    cl_uint num_compute_units = device.GetRef().getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
    cl_uint adjust_local_size =
        (cl_uint) cluster_adjust_kernel.GetBackingRef().getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device.GetRef());
    cl::NDRange adjust_local_range(adjust_local_size);
    cl::NDRange adjust_global_range(adjust_local_size*num_compute_units);

    cl::LocalSpaceArg local_center_buffer = cl::Local(adjust_local_size*num_centroids*sizeof(cl_float4));
    cl::LocalSpaceArg local_count_buffer = cl::Local(adjust_local_size*num_centroids*sizeof(cl_uint));

    OCLBuffer<float>* work_group_center_buffer = manager_->CreateBuffer<float>(num_centroids*num_compute_units*4, this);
    OCLBuffer<cl_uint>* work_group_count_buffer = manager_->CreateBuffer<cl_uint>(num_centroids*num_compute_units, this);

    success = cluster_adjust_kernel.SetArgs(points, point_classifications, num_points, centroids, num_centroids,
        local_center_buffer, local_count_buffer, *work_group_center_buffer, *work_group_count_buffer);
    if (!success) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to set args for cluster adjust kernel, skipping");
        return false;
    }
    
    for (int iter=0; iter < iterations; iter++) {
        // Identify the nearest centroid for each point
        success = cluster_classify_kernel.Run(device, true, cl::NullRange, global_classify_threads, cl::NullRange);
        if (!success) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "Failed to execute K-means classification kernel");
            return false;
        }

        // Find the averaged position of all the points associated with each given centroid
        success = cluster_adjust_kernel.Run(device, true, cl::NullRange, adjust_global_range, adjust_local_range);
        if (!success) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "Failed to execute K-means center adjustment kernel");
            return false;
        }

        //Read back the per-work-group result from the adjust kernel
        auto wg_center_vec = work_group_center_buffer->ReadData(device, true, this);
        auto wg_count_vec = work_group_count_buffer->ReadData(device, true, this);
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
        for (int i=0; i<wg_count_vec.size(); i+=1) {
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
        centroids.WriteData(new_centroids, device, true);
    }

    load_balancer_->ReleaseDevice(device_id);

    return true;
}

void OpenCLWorker::Log(std::string function_name, ModelLogger::WorkloadEvent event, int work_id, std::string args) {
    Worker::Log("OpenCLWorker::"+function_name, event, work_id, args);
    std::cerr << "OpenCLWorker::" << function_name << ", " << args << std::endl;
}

OpenCLKernel* OpenCLWorker::InitKernel(OpenCLManager& manager, std::string kernel_name, std::string source_file) {
    std::vector<std::string> filenames;
    filenames.push_back(source_file);

    auto kernel_vec = manager_->CreateKernels(filenames, this);
    if (kernel_vec.size() == 0) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
        "Unable to find any kernels in "+source_file);
        return NULL;
    }
    
    bool found_kernel = false;
    for (auto& kernel : kernel_vec) {
        if (kernel.GetName() == kernel_name) {
            found_kernel = true;
            return new OpenCLKernel(kernel);
        }
    }

    Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
        "Unable to find a kernel called "+kernel_name+" in "+source_file);
    return NULL;
}

OpenCLKernel& OpenCLWorker::GetKernel(OpenCLDevice& device, const std::string& kernel_name, const std::string& source_file) {
    for (auto& kernel_wrapper : device.GetKernels()) {
        auto& kernel = kernel_wrapper.get();
        if (kernel.GetName() == kernel_name) {
            return kernel;
        }
    }

    std::vector<std::string> filenames;
    filenames.push_back(source_file);
    device.LoadKernelsFromSource(filenames, *this);

    auto kernel_vec = device.GetKernels();
    if (kernel_vec.size() == 0) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
        "Unable to find any kernels in "+source_file);
        throw std::invalid_argument("No kernels in file "+source_file);
    }
    
    for (auto& kernel_wrapper : kernel_vec) {
        auto& kernel = kernel_wrapper.get();
        if (kernel.GetName() == kernel_name) {
            return kernel;
        }
    }

    Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
        "Unable to find a kernel called "+kernel_name+" in "+source_file);
    throw std::invalid_argument("Unable to find kernel with name "+kernel_name+" in file "+source_file);
}