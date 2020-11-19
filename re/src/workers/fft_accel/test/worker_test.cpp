//
// Created by Jackson on 10/11/20.
//

#include "gtest/gtest.h"

#include "sem_fft_accel_worker.hpp"

#include "component.h"

using namespace sem::fft_accel;

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

using test_worker_type = sem::fft_accel::Worker;

TEST(fft_accel_worker, nothrow_constructor) {
    std::unique_ptr<sem::fft_accel::Worker> worker_ptr;
    // Construct dependencies
    const Component test_component = generate_component();
    std::string worker_inst_name = generate_worker_inst_name();

    ASSERT_NO_THROW(
            worker_ptr = std::make_unique<test_worker_type>(test_component, worker_inst_name)
                    );

    ASSERT_NE(worker_ptr, nullptr);
}

TEST(fft_accel_worker, ip_address_configured_from_attributes) {
    // Construct dependencies
    const Component test_component = generate_component();
    std::string worker_inst_name = generate_worker_inst_name();

    // Construct worker under test
    std::unique_ptr<sem::fft_accel::Worker> worker;
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