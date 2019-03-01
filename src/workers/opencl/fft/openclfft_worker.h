#ifndef OPENCLFFT_H
#define OPENCLFFT_H

#include <core/worker.h>

class OpenCLManager;
class OpenCLLoadBalancer;
class OpenCLDevice;

struct clfftSetupData_;
typedef struct clfftSetupData_ clfftSetupData;

class OpenCLFFTWorker : public Worker{
public:
    OpenCLFFTWorker(const Component& component, std::string inst_name);
    ~OpenCLFFTWorker();
    const std::string& get_version() const override;

    bool HandleConfigure();

    bool FFT(std::vector<float> &data);

protected:
    virtual void Log(std::string function_name, Logger::WorkloadEvent event, int work_id = -1, std::string args = "");

private:
    
    bool is_valid_ = false;

    OpenCLManager* manager_ = NULL;
    OpenCLLoadBalancer* load_balancer_ = NULL;

    std::vector<std::reference_wrapper<OpenCLDevice> > devices_;

    std::shared_ptr<Attribute> platform_id_;
    std::shared_ptr<Attribute> device_id_;

    // FFT plan handle
    clfftSetupData* fftSetupData;

	void InitCLFFT();
	void CleanupCLFFT();
};

#endif