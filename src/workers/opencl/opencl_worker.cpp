#include "opencl_worker.h"
#include "openclutilities.h"

#include <math.h>
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
    //auto result_buffer = manager_->CreateBuffer<float>(matC.size(), this);
    auto result_buffer = manager_->CreateBuffer<float>(matC, this);

    bool success = MatrixMult(*bufferA, *bufferB, *result_buffer);
    auto new_matA = bufferA->ReadData(true, this);
    auto new_matB = bufferB->ReadData(true, this);
    matC = result_buffer->ReadData(true, this);

    manager_->ReleaseBuffer(bufferA, this);
    manager_->ReleaseBuffer(bufferB, this);
    manager_->ReleaseBuffer(result_buffer, this);

    return success;
}

bool OpenCLWorker::MatrixMult(const OCLBuffer<float>& matA, const OCLBuffer<float>& matB, OCLBuffer<float>& matC) {
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
    //std::cout << "Block length: " << block_length << std::endl;


    //size_t block_data_size = block_length*block_length*sizeof(cl_float);
    cl::LocalSpaceArg block_data_size = cl::Local(block_length*block_length*sizeof(cl_float));

    unsigned int max_mat_width = std::max(M, K);
    unsigned int max_mat_height = std::max(N, K);

    unsigned int global_width = 0;
	while (global_width < max_mat_width) global_width += block_length;
	unsigned int global_height = 0;
	while (global_height < max_mat_height) global_height += block_length;

    // TODO: Mutex this stuff
    bool did_set_args = matrix_kernel_->SetArgs(matA, matB, matC, M, K, N,
        block_data_size, block_data_size);
        //cl::Local(block_data_size), cl::Local(block_data_size));
    if (!did_set_args) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to set args for MatrixMult");
            return false;
    }

    bool did_kernel_complete = matrix_kernel_->Run(0, true, cl::NullRange,
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
    
    if (cluster_classify_kernel_ == NULL) {
        auto filename = GetSourcePath("kmeans.cl");
        
        cluster_classify_kernel_ = InitKernel(*manager_, "classifyPoints", filename);
        if (cluster_classify_kernel_ == NULL) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "Failed to initialise classifyPoints kernel, skipping");
                return false;
        }
    }

    if (cluster_adjust_kernel_ == NULL) {
        auto filename = GetSourcePath("kmeans.cl");
        
        cluster_adjust_kernel_ = InitKernel(*manager_, "adjustCentroids", filename);
        if (cluster_adjust_kernel_ == NULL) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "Failed to initialise adjustCentroids kernel, skipping");
                return false;
        }
    }

    cl_uint num_centroids = (cl_uint) centroids.GetNumElements() / (sizeof(cl_float4) / sizeof(float));
    cl_uint num_points = (cl_uint) points.GetNumElements() / (sizeof(cl_float4) / sizeof(float));
    cl_uint num_classes = (cl_uint) point_classifications.GetNumElements();
    if (num_points != num_classes) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Attempting to run K-means cluster algorithm, but the point and classification vector lengths don't match");
        return false;
    }

    // std::vector<cl_float4> center_vec;
    // center_vec.push_back({0,0,0,0});
    // center_vec.push_back({1,1,1,1});
    // OCLBuffer<cl_float4>* test_centroids = new OCLBuffer<cl_float4>(*manager_, center_vec, this);

    //cluster_classify_kernel_->GetBackingRef().setArg()

    // Calculate classification parameters
    cl::NDRange global_classify_threads(num_points);
    success = cluster_classify_kernel_->SetArgs(points, point_classifications, centroids, num_centroids);
    if (!success) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to set args for cluster classify kernel, skipping");
        return false;
    }

    // Calculate adjust parameters
    cl::Device dev = manager_->GetContext().getInfo<CL_CONTEXT_DEVICES>()[0];
    cl_uint num_compute_units = dev.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
    cl_uint adjust_local_size =
        (cl_uint) cluster_adjust_kernel_->GetBackingRef().getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(dev);
    cl::NDRange adjust_local_range(adjust_local_size);
    cl::NDRange adjust_global_range(adjust_local_size*num_compute_units);

    cl::LocalSpaceArg local_center_buffer = cl::Local(adjust_local_size*num_centroids*sizeof(cl_float4));
    cl::LocalSpaceArg local_count_buffer = cl::Local(adjust_local_size*num_centroids*sizeof(cl_uint));

    OCLBuffer<float>* work_group_center_buffer = manager_->CreateBuffer<float>(num_centroids*num_compute_units*4, this);
    OCLBuffer<cl_uint>* work_group_count_buffer = manager_->CreateBuffer<cl_uint>(num_centroids*num_compute_units, this);

    success = cluster_adjust_kernel_->SetArgs(points, point_classifications, num_points, centroids, num_centroids,
        local_center_buffer, local_count_buffer, *work_group_center_buffer, *work_group_count_buffer);
    if (!success) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
            "Failed to set args for cluster adjust kernel, skipping");
        return false;
    }
    
    for (int iter=0; iter < iterations; iter++) {
        // Identify the nearest centroid for each point
        success = cluster_classify_kernel_->Run(0, true, cl::NullRange, global_classify_threads, cl::NullRange);
        if (!success) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "Failed to execute K-means classification kernel");
            return false;
        }

        // Find the averaged position of all the points associated with each given centroid
        success = cluster_adjust_kernel_->Run(0, true, cl::NullRange, adjust_global_range, adjust_local_range);
        if (!success) {
            Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(), 
                "Failed to execute K-means center adjustment kernel");
            return false;
        }

        //Read back the per-work-group result from the adjust kernel
        auto wg_center_vec = work_group_center_buffer->ReadData(true, this);
        auto wg_count_vec = work_group_count_buffer->ReadData(true, this);
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
            //std::cout << wg_center_vec[i*4+0] << ',' << wg_center_vec[i*4+1] << ',' << wg_center_vec[i*4+2] << ',' << wg_center_vec[i*4+3] << std::endl;
            //std::cout << wg_count_vec[i] << std::endl;
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
        centroids.WriteData(new_centroids, true);
    }

    return true;
}

void OpenCLWorker::Log(std::string function_name, ModelLogger::WorkloadEvent event, int work_id, std::string args) {
    Worker::Log("OpenCLWorker::"+function_name, event, work_id, args);
    std::cerr << "OpenCLWorker::" << function_name << ", " << args << std::endl;
}

OpenCLKernel* OpenCLWorker::InitKernel(OpenCLManager& manager, std::string kernel_name, std::string source_file) {
    std::vector<std::string> filenames;// = {source_file};
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

    //if (!found_kernel) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
            "Unable to find a kernel called "+kernel_name+" in "+source_file);
        return NULL;
    //}
}