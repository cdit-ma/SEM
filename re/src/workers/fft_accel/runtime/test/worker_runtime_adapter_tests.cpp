//
// Created by Jackson Michael on 26/8/20.
//

#include "gtest/gtest.h"

#include "runtime/worker_adapter.hpp"

#include "mock_network_adapter.hpp"

#include "component.h"

using namespace sem;
using namespace sem::fft_accel;
using namespace sem::fft_accel::test::worker;

Component generate_component() {
    std::string comp_inst_name = "comp_inst_name";
    return Component(comp_inst_name);
}

std::string generate_worker_inst_name() {
    return "worker_inst_name";
}

std::string generate_FAE_endpoint() {
    return "123.234.123.234:37256";
}

using test_worker_type = runtime::worker_adapter;

TEST(fft_accel_worker, nothrow_constructor) {
    std::unique_ptr<runtime::adapter> runtime_adapter;

    const Component test_component = generate_component();
    std::string worker_inst_name = generate_worker_inst_name();
    ASSERT_NO_THROW(
            runtime_adapter = std::make_unique<test_worker_type>(
                    test_component, worker_inst_name
                    );
            );
    ASSERT_NE(runtime_adapter, nullptr);
}

TEST(fft_accel_worker, network_adapter_registered_when_set) {
    // Construct dependencies
    const Component test_component = generate_component();
    std::string worker_inst_name = generate_worker_inst_name();

    // Construct worker under test
    std::unique_ptr<runtime::adapter> worker;
    worker = std::make_unique<test_worker_type>(
            test_component, worker_inst_name
    );

    // Construct mocked network adapter and set its expectations
    auto mock_adapter = std::make_shared<mock_network_adapter<float>>();
    ASSERT_NE(mock_adapter, nullptr);
    EXPECT_CALL(*mock_adapter, register_listener);

    // Perform the set
    try {
        ASSERT_EQ(worker->set_network_adapter(mock_adapter), Result<void>{});
    } catch (const std::exception& ex) {
        FAIL() << "An exception was thrown when set_network_adapter was called: " << ex.what();
    } catch (...) {
        FAIL() << "A non-std::exception type was thrown when set_network_adapter was called";
    }
}

TEST(fft_accel_worker, ip_address_configured_from_attributes) {
    // Construct dependencies
    const Component test_component = generate_component();
    std::string worker_inst_name = generate_worker_inst_name();

    // Construct worker under test
    std::unique_ptr<runtime::adapter> worker;
    worker = std::make_unique<test_worker_type>(
            test_component, worker_inst_name
            );

    // Set the appropriate attributes
    std::string attr_name{test_worker_type::AttrNames::accelerator_endpoint};
    std::shared_ptr<Attribute> attr;
    ASSERT_NO_THROW(
            attr = dynamic_cast<test_worker_type*>(worker.get())->ConstructAttribute(
                    ATTRIBUTE_TYPE::STRING, attr_name
                    ).lock();
        );
    ASSERT_NE(attr, nullptr);
    ASSERT_NO_THROW(
            attr->set_String(generate_FAE_endpoint());
        );

    ASSERT_NO_THROW(
            dynamic_cast<test_worker_type*>(worker.get())->Configure();
            );


}