#include "opencl_worker.h"

/**
 * Provides stub implementations for all FFT functions in the case that no FFT libraries could be
 * found during compilation
 **/

bool OpenCL_Worker::InitFFT(int work_id) { return false; };
bool OpenCL_Worker::CleanupFFT(int work_id) {return false; };


bool OpenCL_Worker::FFT(std::vector<float> &data) {
    Log(std::string(__func__), Logger::WorkloadEvent::ERROR, get_new_work_id(),
        "Unable to run FFT; no implementation provided. Please install an FFT library");
    return false;
}
bool OpenCL_Worker::FFT(OpenCLBuffer<float> &data, int device_id, int work_id) {
    Log(std::string(__func__), Logger::WorkloadEvent::ERROR, work_id,
        "Unable to run FFT; no implementation provided. Please install an FFT library");
    return false;
}
