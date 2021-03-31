//
// Created by Jackson Michael on 10/11/20.
//

#include "gtest/gtest.h"

#include "sem_fft_accel_worker.h"

#include "component.h"
#include "print_logger.h"

#include "data/test/data_test_util.hpp"
#include "udp_packet_loopback.hpp"
#include "runtime/test/callback_handler.hpp"

#include <functional>


using namespace sem::fft_accel;
using namespace testing;

using namespace std::chrono_literals;

Component generate_component() {
    std::string comp_inst_name = "fft_accel_test_component";
    return Component(comp_inst_name);
}

std::string generate_worker_inst_name() {
    return "test_worker_inst_name";
}

std::string generate_FAE_endpoint() {
    return "123.234.123.234:37256";
}

using test_runtime_adapter_type = sem::fft_accel::Worker;

TEST(fft_accel_worker, nothrow_constructor) {
    std::unique_ptr<sem::fft_accel::Worker> worker_ptr;
    // Construct dependencies
    const Component test_component = generate_component();
    std::string worker_inst_name = generate_worker_inst_name();

    ASSERT_NO_THROW(
            worker_ptr = std::make_unique<test_runtime_adapter_type>(test_component, worker_inst_name)
    );

    ASSERT_NE(worker_ptr, nullptr);
}

TEST(fft_accel_worker, ip_address_configured_from_attributes) {
    // Construct dependencies
    const Component test_component = generate_component();
    std::string worker_inst_name = generate_worker_inst_name();

    // Construct worker under test
    std::unique_ptr<sem::fft_accel::Worker> worker;
    worker = std::make_unique<test_runtime_adapter_type>(
            test_component, worker_inst_name
    );

    // Set the appropriate attributes
    std::string attr_name{test_runtime_adapter_type::AttrNames::accelerator_endpoint};
    std::shared_ptr<Attribute> attr;
    ASSERT_NO_THROW(
            attr = worker->ConstructAttribute(
                    ATTRIBUTE_TYPE::STRING, attr_name
            ).lock();
    );
    ASSERT_NE(attr, nullptr);
    ASSERT_NO_THROW(
            attr->set_String(generate_FAE_endpoint());
    );

    ASSERT_NO_THROW(
            worker->Configure();
    );

}

TEST(fft_accel_worker, calculate_fft_through_loopback) {
    // Construct dependencies
    const Component test_component = generate_component();
    std::string worker_inst_name = generate_worker_inst_name();
    Print::Logger logger;

    // Construct packet looper
    test::udp_packet_loopback udp_loopback;
    re::types::SocketAddress loopback_addr(re::types::Ipv4::localhost(), udp_loopback.get_port());

    // Construct worker under test
    std::unique_ptr<sem::fft_accel::Worker> worker;
    worker = std::make_unique<test_runtime_adapter_type>(test_component, worker_inst_name);

    // Set the appropriate attributes
    std::string attr_name{test_runtime_adapter_type::AttrNames::accelerator_endpoint};
    auto attr = worker->ConstructAttribute(ATTRIBUTE_TYPE::STRING, attr_name).lock();
    attr->set_String(loopback_addr.to_string());

    worker->logger().AddLogger(logger);
    worker->Configure();

    auto input_data = data::test::generate_random_single_packet_fft_vec_data();
    std::vector<float> output_data;
    //ASSERT_NO_THROW(output_data = worker->calculate_fft(input_data));
    output_data = worker->calculate_fft(input_data);

    ASSERT_EQ(input_data.size(), output_data.size());
    /*
    // Uncomment below lines for more precise debugging of failing indices
    for (int i=0; i < input_data.size(); i++) {
        EXPECT_EQ(input_data.at(i), output_data.at(i)) << "index value is: " << i;
    }
     */
    ASSERT_EQ(input_data, output_data);
}

TEST(fft_accel_worker, calculate_fft_async_and_receive_through_loopback) {
    // Construct dependencies
    const Component test_component = generate_component();
    std::string worker_inst_name = generate_worker_inst_name();
    Print::Logger logger;
    testing::MockFunction<void(uint8_t, std::vector<float>)> mock_callback;

    auto input_data = data::test::generate_random_single_packet_fft_vec_data();

    // Construct packet looper
    test::udp_packet_loopback udp_loopback;
    re::types::SocketAddress loopback_addr(re::types::Ipv4::localhost(), udp_loopback.get_port());


    // Construct worker under test
    std::unique_ptr<sem::fft_accel::Worker> worker;
    worker = std::make_unique<test_runtime_adapter_type>(test_component, worker_inst_name);

    // Set the appropriate attributes
    std::string attr_name{test_runtime_adapter_type::AttrNames::accelerator_endpoint};
    auto attr = worker->ConstructAttribute(ATTRIBUTE_TYPE::STRING, attr_name).lock();
    attr->set_String(loopback_addr.to_string());

    worker->logger().AddLogger(logger);

    uint8_t callback_request_id;
    std::promise<std::vector<float>> callback_promise;
    EXPECT_CALL(mock_callback, Call(_,input_data)).WillOnce(Invoke(
            [&callback_promise, &callback_request_id](uint8_t id, std::vector<float> vec) {
                callback_request_id = id;
                callback_promise.set_value(std::move(vec));
            }
    ));

    ASSERT_NO_THROW(
            worker->SetResponseCallback(mock_callback.AsStdFunction());
    );

    ASSERT_TRUE(worker->Configure());

    uint16_t output_id;
    ASSERT_NO_THROW(output_id = worker->calculate_fft_async(input_data));

    auto callback_future = callback_promise.get_future();
    ASSERT_EQ(callback_future.wait_for(300ms), std::future_status::ready);
    auto callback_result = callback_future.get();
    ASSERT_EQ(callback_result, input_data);

    ASSERT_EQ(callback_request_id, output_id);
}
