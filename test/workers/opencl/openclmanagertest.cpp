#include "gtest/gtest.h"

#include <vector>
#include <iostream>
#include <random>
#include <workers/opencl/openclutilities.h>
#include <workers/opencl/openclmanager.h>
#include <workers/opencl/opencl_worker.h>

#include "common.h"

//This is our test case for the frame
TEST(OpenCLManager, GetDevices)
{
    Component component("dummy_component");
    Worker worker(component, "dummy_worker", "dummy_worker");

    auto platforms = OpenCLManager::GetPlatforms(worker);

    //Test for platforms
    ASSERT_GT(platforms.size(), 0);

    for(int platform_index = 0; platform_index < platforms.size(); platform_index ++){
        auto manager = OpenCLManager::GetReferenceByPlatformID(worker, platform_index);
        PrintInfo("Platform " + std::to_string(platform_index) + ": " + platforms[platform_index].getInfo<CL_PLATFORM_NAME>());
        ASSERT_TRUE(manager);

        auto& devices = manager->GetDevices(worker);
        ASSERT_GT(devices.size(), 0);

        for(const auto& device : devices){
            PrintInfo("* Device: " + device->GetName());
        }
    }
}


class OpenCLManagerFixture : public ::testing::TestWithParam<DeviceParam>{
    public:
        OpenCLManagerFixture():
            component_("dummy_component"),
            worker_(component_, "dummy_worker", "dummy_worker")
        {
            device_ = GetParam();
            RecordProperty("Device_ID", device_.device_id);
            RecordProperty("Platform_ID", device_.platform_id);

            auto platforms = OpenCLManager::GetPlatforms(worker_);
            if(platforms.size() == 0){
                throw std::runtime_error("No OpenCL platforms detected during OpenCLManagerFixture construction");
            }
            
            manager_ = OpenCLManager::GetReferenceByPlatformID(worker_, device_.platform_id);
            if(!manager_){
                throw std::runtime_error("Failed to obtain OpenCLManagerReference during OpenCLManagerFixture construction");
            }
        };

        OpenCLDevice* GetDevice(){
            //TODO: Add a index based device getter to OpenCLManager
            OpenCLDevice* dev = manager_->GetDevices(worker_).at(device_.device_id).get();
            return dev;
        };

        DeviceParam device_;
        Component component_;
        Worker worker_;
        OpenCLManager* manager_;
};

TEST_P(OpenCLManagerFixture, BufferReadWrite_Float4)
{
    auto buffer = manager_->CreateBuffer<float>(worker_, 4);
    ASSERT_NE(buffer, nullptr);
    
    auto device = GetDevice();
    ASSERT_NE(device, nullptr);

    ASSERT_TRUE(buffer->is_valid());

    auto in_data = std::vector<float>(4);
    for (int i = 0; i < in_data.size(); i++) {
        in_data[i] = (float) i + 10;
    }
    in_data[0] = (2.0f/3.0f);

    ASSERT_TRUE(buffer->WriteData(worker_, in_data, *device));
    auto out_data = buffer->ReadData(worker_, *device);
    EXPECT_EQ(in_data, out_data);
    
    manager_->ReleaseBuffer(worker_, buffer);
}

TEST_P(OpenCLManagerFixture, BufferReadWrite_FloatRandom1024)
{ 
    std::default_random_engine random_generator;
    std::default_random_engine generator(testing::UnitTest::GetInstance()->random_seed());
    std::uniform_real_distribution<float> distribution(-1, 1);

    auto buffer = manager_->CreateBuffer<float>(worker_, 1024);

    ASSERT_NE(buffer, nullptr);

    auto device = GetDevice();
    ASSERT_NE(device, nullptr);
    std::cerr << device << std::endl;
    std::cerr << device->GetName() << std::endl;

    ASSERT_TRUE(buffer->is_valid());

    auto in_data = std::vector<float>(1024);
    for (int i = 0; i < in_data.size(); i++) {
        in_data[i] = distribution(generator);
    }

    ASSERT_TRUE(buffer->WriteData(worker_, in_data, *device));
    std::cerr << "got past write" << std::endl;
    auto out_data = buffer->ReadData(worker_, *device);
    EXPECT_EQ(in_data, out_data);
    
    manager_->ReleaseBuffer(worker_, buffer);
    //delete buffer;
}

TEST_P(OpenCLManagerFixture, BufferKernelPassthrough_Float4)
{
    auto device = GetDevice();
    ASSERT_NE(device, nullptr);
    size_t size = 4;
    
    auto in_data = std::vector<float>(size, 2.0f/3.0f);
    auto in_buffer = manager_->CreateBuffer<float>(worker_, in_data, *device);
    ASSERT_NE(in_buffer, nullptr);
    auto out_buffer = manager_->CreateBuffer<float>(worker_, size);
    ASSERT_NE(out_buffer, nullptr);

    ASSERT_TRUE(in_buffer->is_valid());
    ASSERT_TRUE(out_buffer->is_valid());
    //Load the kernel
    ASSERT_TRUE(device->LoadKernelsFromSource(worker_, {GetSourcePath("kernel.cl")}));

    //TODO: Jackson Add a kernel getter for a device which takes the name of the kernel, and throws exception if it can't find it
    OpenCLKernel* passthrough_kernel = NULL;
    for (auto& kernel_refwrapper : device->GetKernels()) {
        auto& kernel = kernel_refwrapper.get();
        if (kernel.GetName() == "dataPassthroughTest") {
            passthrough_kernel = &kernel;
        }
    }
    ASSERT_FALSE(passthrough_kernel == NULL);

    passthrough_kernel->SetArgs((*in_buffer), (*out_buffer));
    passthrough_kernel->Run(*device, true, cl::NullRange, cl::NDRange(size), cl::NDRange(size));

    auto out_data = out_buffer->ReadData(worker_, *device);
    EXPECT_EQ(in_data, out_data);

    manager_->ReleaseBuffer(worker_, in_buffer);
    manager_->ReleaseBuffer(worker_, out_buffer);
}

TEST(LoadBalancer, Simple3Device)
{
    OpenCLLoadBalancer load_balancer({1, 2, 3});

    auto j1 = load_balancer.RequestDevice();
    auto j2 = load_balancer.RequestDevice();
    auto j3 = load_balancer.RequestDevice();

    //Check that all the jobs have differing devices assigned
    EXPECT_NE(j1, j2);
    EXPECT_NE(j2, j3);
    EXPECT_NE(j3, j1);

    auto j4 = load_balancer.RequestDevice();
    auto j5 = load_balancer.RequestDevice();
    auto j6 = load_balancer.RequestDevice();

    //Check that all the jobs have differing devices assigned
    EXPECT_NE(j4, j5);
    EXPECT_NE(j5, j6);
    EXPECT_NE(j6, j4);

    load_balancer.ReleaseDevice(j4);

    auto j7 = load_balancer.RequestDevice();

    //Check that j7 took the device that j4 released
    EXPECT_EQ(j7, j4);
}

//Run the OpenCLManagerTest Fixture with all devices available
INSTANTIATE_TEST_CASE_P(Test, OpenCLManagerFixture, ::testing::ValuesIn(getDevices()));
