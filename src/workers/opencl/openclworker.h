#ifndef OPENCLWORKER_H
#define OPENCLWORKER_H

#include <core/worker.h>
#include "openclmanager.h"
#include "openclkernel.hpp"

class OpenCLWorker : public Worker {
public:
    OpenCLWorker(Component* component, std::string inst_name, int platform_id);
    ~OpenCLWorker();

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
    void RunParallel(int num_threads, int ops_per_thread);
    void MatrixMult(const std::vector<float>& matA, const std::vector<float>& matB, std::vector<float>& matC);
    std::vector<int> KmeansCluster(const std::vector<float>& points, std::vector<float>& centroids, int iterations);


private:
    OpenCLManager* manager_ = NULL;
    
    OpenCLKernel* parallel_kernel_ = NULL;
    OpenCLKernel* matrix_kernel_ = NULL;
    OpenCLKernel* cluster_classify_kernel_ = NULL;
    OpenCLKernel* cluster_adjust_kernel_ = NULL;

};

#endif