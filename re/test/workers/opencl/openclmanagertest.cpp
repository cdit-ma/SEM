#include "gtest/gtest.h"

#include <iostream>
#include <random>
#include <vector>
#include "manager.h"
#include "opencl_worker.h"
#include "utilities.h"

#include "common.h"
#include "print_logger.h"

using namespace Re::OpenCL;

// This is our test case for the frame
TEST(Re_Workers_OpenclWorker, Manager_GetDevices)
{
    Component component("dummy_component");
    DummyWorker worker(component);

    auto platforms = Manager::GetPlatforms();

    // Test for platforms
    ASSERT_GT(platforms.size(), 0);

    for(int platform_index = 0; platform_index < platforms.size(); platform_index++) {
        auto manager = Manager::GetReferenceByPlatformID(platform_index, &worker);
        PrintInfo("Platform " + std::to_string(platform_index) + ": "
                  + platforms[platform_index].getInfo<CL_PLATFORM_NAME>());
        ASSERT_TRUE(manager);

        auto& devices = manager->GetDevices();
        ASSERT_GT(devices.size(), 0);

        for(const auto& device : devices) {
            PrintInfo("* Device: " + device->GetName());
        }
    }
}

class ManagerFixture : public ::testing::TestWithParam<DeviceParam> {
    public:
    ManagerFixture() : component_("dummy_component"), worker_(component_)
    {
        Print::Logger::get_logger().SetLogLevel(10);
        worker_.logger().AddLogger(Print::Logger::get_logger());
        device_ = GetParam();
        RecordProperty("Device_ID", device_.device_id);
        RecordProperty("Platform_ID", device_.platform_id);

        auto platforms = Manager::GetPlatforms();
        if(platforms.size() == 0) {
            throw std::runtime_error("No OpenCL platforms detected during ManagerFixture construction");
        }

        manager_ = Manager::GetReferenceByPlatformID(device_.platform_id, &worker_);
        if(!manager_) {
            throw std::runtime_error("Failed to obtain ManagerReference during ManagerFixture construction");
        }
    };

    Device* GetDevice()
    {
        // TODO: Add a index based device getter to Manager
        auto& dev = manager_->GetDevices().at(device_.device_id);
        return dev.get();
    };

    DeviceParam device_;
    Component component_;
    DummyWorker worker_;
    Manager* manager_;
};

TEST_P(ManagerFixture, BufferReadWrite_Float4)
{
    auto buffer = manager_->CreateBuffer<float>(4);

    auto device = GetDevice();
    ASSERT_NE(device, nullptr);

    ASSERT_TRUE(buffer.IsValid());

    auto in_data = std::vector<float>(4);
    for(int i = 0; i < in_data.size(); i++) {
        in_data[i] = (float)i + 10;
    }
    in_data[0] = (2.0f / 3.0f);

    ASSERT_TRUE(buffer.WriteData(in_data, *device));
    auto out_data = buffer.ReadData(*device);
    EXPECT_EQ(in_data, out_data);

    manager_->ReleaseBuffer(buffer);
}

TEST_P(ManagerFixture, BufferReadWrite_FloatRandom1024)
{
    std::default_random_engine random_generator;
    std::default_random_engine generator(testing::UnitTest::GetInstance()->random_seed());
    std::uniform_real_distribution<float> distribution(-1, 1);

    auto buffer = manager_->CreateBuffer<float>(1024);

    auto device = GetDevice();
    ASSERT_NE(device, nullptr);

    ASSERT_TRUE(buffer.IsValid());

    auto in_data = std::vector<float>(1024);
    for(int i = 0; i < in_data.size(); i++) {
        in_data[i] = distribution(generator);
    }

    ASSERT_TRUE(buffer.WriteData(in_data, *device));
    std::vector<float> out_data;
    EXPECT_NO_THROW(out_data = buffer.ReadData(*device));
    EXPECT_EQ(in_data, out_data);

    manager_->ReleaseBuffer(buffer);
}

TEST_P(ManagerFixture, BufferKernelPassthrough_Float4)
{
    auto device = GetDevice();
    ASSERT_NE(device, nullptr);
    size_t size = 4;

    auto in_data = std::vector<float>(size, 2.0f / 3.0f);
    auto in_buffer = manager_->CreateBuffer<float>(in_data, *device);
    auto out_buffer = manager_->CreateBuffer<float>(size);

    ASSERT_TRUE(in_buffer.IsValid());
    ASSERT_TRUE(out_buffer.IsValid());
    // Load the kernel
    ASSERT_NO_THROW(device->LoadKernelsFromSource({GetSourcePath("kernel.cl")}));

    for(const auto& kernels : device->GetKernels()) {
    }

    // TODO: Jackson Add a kernel getter for a device which takes the name of the kernel, and throws exception if it
    // can't find it
    Kernel* passthrough_kernel = NULL;
    EXPECT_NO_THROW(passthrough_kernel = &device->GetKernel("dataPassthroughTest"));
    ASSERT_FALSE(passthrough_kernel == NULL);

    EXPECT_NO_THROW(passthrough_kernel->SetArgs((in_buffer), (out_buffer)));
    EXPECT_NO_THROW(passthrough_kernel->Run(*device, true, cl::NullRange, cl::NDRange(size), cl::NDRange(size)));

    std::vector<float> out_data;
    EXPECT_NO_THROW(out_data = out_buffer.ReadData(*device));
    EXPECT_EQ(in_data, out_data);

    EXPECT_NO_THROW(manager_->ReleaseBuffer(in_buffer));
    EXPECT_NO_THROW(manager_->ReleaseBuffer(out_buffer));
}

TEST(Re_Worker_OpenclWorker, LoadBalancer_Simple3Device)
{
    LoadBalancer load_balancer({1, 2, 3});

    auto j1 = load_balancer.RequestDevice();
    auto j2 = load_balancer.RequestDevice();
    auto j3 = load_balancer.RequestDevice();

    // Check that all the jobs have differing devices assigned
    EXPECT_NE(j1, j2);
    EXPECT_NE(j2, j3);
    EXPECT_NE(j3, j1);

    auto j4 = load_balancer.RequestDevice();
    auto j5 = load_balancer.RequestDevice();
    auto j6 = load_balancer.RequestDevice();

    // Check that all the jobs have differing devices assigned
    EXPECT_NE(j4, j5);
    EXPECT_NE(j5, j6);
    EXPECT_NE(j6, j4);

    load_balancer.ReleaseDevice(j4);

    auto j7 = load_balancer.RequestDevice();

    // Check that j7 took the device that j4 released
    EXPECT_EQ(j7, j4);
}

// Run the ManagerTest Fixture with all devices available
INSTANTIATE_TEST_CASE_P(Re_Worker_OpenclWorker_Manager, ManagerFixture, ::testing::ValuesIn(getDevices()));
