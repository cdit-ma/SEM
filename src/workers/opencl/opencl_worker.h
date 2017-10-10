#ifndef OPENCLWORKER_H
#define OPENCLWORKER_H

#include <core/worker.h>
#include "openclmanager.h"
#include "oclbuffer.hpp"
#include "openclkernel.hpp"

class OpenCLWorker : public Worker {
public:
    OpenCLWorker(Component* component, std::string inst_name, int platform_id=0);
    ~OpenCLWorker();

    bool IsValid() const;

    // Base/Utility functions
    template <typename T>
    OCLBuffer<T>* CreateBuffer(std::vector<T> data);
    template <typename T>
    void ReleaseBuffer(OCLBuffer<T>* buffer);
    template <typename T>
    void WriteBuffer(OCLBuffer<T>* buffer);
    template <typename T>
    std::vector<T> ReadBuffer(OCLBuffer<T>* buffer);

    // Bespoke algorithms
    bool RunParallel(int num_threads, long long ops_per_thread);
    bool MatrixMult(const OCLBuffer<float>& matA, const OCLBuffer<float>& matB, OCLBuffer<float>& matC);
    bool MatrixMult(const std::vector<float>& matA, const std::vector<float>& matB, std::vector<float>& matC);
    std::vector<int> KmeansCluster(const std::vector<float>& points, std::vector<float>& centroids, int iterations);


private:

    OpenCLKernel* InitKernel(OpenCLManager& manager, std::string kernel_name, std::string source_file);

    bool is_valid_ = false;

    OpenCLManager* manager_ = NULL;
    
    OpenCLKernel* parallel_kernel_ = NULL;
    OpenCLKernel* matrix_kernel_ = NULL;
    OpenCLKernel* cluster_classify_kernel_ = NULL;
    OpenCLKernel* cluster_adjust_kernel_ = NULL;

};

#endif