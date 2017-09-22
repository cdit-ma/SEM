#include "openclworker.h"

OpenCLWorker::OpenCLWorker(Component* component, std::string inst_name, int platform_id)
    : Worker(component, __func__, inst_name) {

    manager_ = OpenCLManager::GetReferenceByPlatform(platform_id);
    if (manager_ = NULL) {
        Log(__func__, ModelLogger::WorkloadEvent::MESSAGE, get_new_work_id(),
            "Unable to obtain a reference to an OpenCLManager");
    }


}

OpenCLWorker::~OpenCLWorker() {
    
}

void OpenCLWorker::RunParallel(int numthreads, int ops_per_thread) {
    if (parallel_kernel_ == NULL) {
        std::string source_file_path(__FILE__);
        auto source_dir = source_file_path.substr(0, source_file_path.find_last_of("/\\")+1);
        std::vector<std::string> filenames = {
            source_dir + "parellelthreads.cl"
        };
        
        auto kernel_vec = manager_->CreateKernels(filenames, this);
        for (auto kernel : kernel_vec) {
            if (kernel.GetName() == "runParallel") {
                parallel_kernel_ = new OpenCLKernel(kernel);
            }
        }
    }
}