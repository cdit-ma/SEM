//
// Created by cdit-ma on 29/03/19.
//
#include <core/component.h>
#include <thread>
#include <workers/tcp/tcp_worker.h>
#include <workers/tcp/tcpworkerimpl.h>

#include "gtest/gtest.h"

// Global port number, incremented inside get_port function
static int port_number = 40000;

int get_port() {
    return ++port_number;
}

std::pair<std::string, std::string> get_test_endpoint() {
    return {"127.0.0.1", std::to_string(get_port())};
}

std::string MessageToString(TcpWorkerImpl::Message message) {
    return {message.begin(), message.end()};
}

TEST(Basic, Constructor) {
    auto component = std::make_shared<Component>("Test");

    Tcp_Worker worker(*component, "worker");
}

// Test for presence and validity of get_version function
TEST(Basic, GetVersion) {
    auto component = std::make_shared<Component>("Test");
    Tcp_Worker worker(*component, "worker");

    EXPECT_TRUE(!worker.get_version().empty());
}

// Connect should fail if there's no valid tcp endpoint to connect to
TEST(Basic, Connect) {
    auto [ip_address, port] = get_test_endpoint();
    auto component = std::make_shared<Component>("Test");

    Tcp_Worker worker(*component, "worker");
    worker.SetAttributeValue("ip_address", ip_address);
    worker.SetAttributeValue("port", port);

    EXPECT_FALSE(worker.Connect());
}

TEST(Basic, Bind) {
    auto [ip_address, port] = get_test_endpoint();
    auto component = std::make_shared<Component>("Test");

    Tcp_Worker worker(*component, "worker");
    worker.SetAttributeValue("ip_address", ip_address);
    worker.SetAttributeValue("port", port);
    worker.SetAttributeValue("delimiter", '\0');

    EXPECT_TRUE(worker.Bind());
    std::this_thread::sleep_for(std::chrono::seconds(1));
    worker.Disconnect();
    worker.Terminate();
}

// Binding callback function should not except/fail
TEST(Basic, BindCallbackFunction) {
    auto component = std::make_shared<Component>("Test");

    Tcp_Worker worker(*component, "worker");

    worker.SetTcpCallback([](std::vector<char> input) {
        std::cout << "RUNNING CALLBACK" << std::endl;
    });
}

TEST(Complex, Callback) {
    std::string long_message = "<test>";
    int send_count = 10;
    auto [ip_address, port] = get_test_endpoint();

    auto rx_component = std::make_shared<Component>("rx_comp");
    Tcp_Worker rx_worker(*rx_component, "rx_worker");
    rx_worker.SetAttributeValue("ip_address", ip_address);
    rx_worker.SetAttributeValue("port", port);
    rx_worker.SetAttributeValue("delimiter", '\0');
    rx_worker.Configure();

    int recd_bytes = 0;
    rx_worker.SetTcpCallback([&recd_bytes, &long_message](std::vector<char> data) {
        recd_bytes += data.size();
        std::string out(data.begin(), data.end());
        EXPECT_EQ(out, long_message);
    });

    auto bind_success = rx_worker.Bind();
    ASSERT_TRUE(bind_success);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto tx_component = std::make_shared<Component>("tx_comp");
    Tcp_Worker tx_worker(*tx_component, "tx_worker");
    tx_worker.SetAttributeValue("ip_address", ip_address);
    tx_worker.SetAttributeValue("port", port);

    auto connect_success = tx_worker.Connect();
    EXPECT_TRUE(connect_success);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    for(int i = 0; i < send_count; ++i) {
        std::vector<char> send_val(long_message.begin(), long_message.end());
        send_val.push_back('\0');
        ASSERT_EQ(tx_worker.Send(send_val), send_val.size());
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    tx_worker.Disconnect();
    rx_worker.Disconnect();

    EXPECT_EQ(recd_bytes, send_count * long_message.size());

    rx_worker.Terminate();
    tx_worker.Terminate();
}

// TODO: This test is currently failing in a CI setting.
// Passes just fine in any context other than jenkins
/*
TEST(Complex, SendReceive) {
    int send_count = 10;
    std::string long_message = "<test>";
    auto [ip_address, port] = get_test_endpoint();

    auto rx_component = std::make_shared<Component>("rx_comp");
    Tcp_Worker rx_worker(*rx_component, "rx_worker");
    rx_worker.SetAttributeValue("ip_address", ip_address);
    rx_worker.SetAttributeValue("port", port);
    rx_worker.SetAttributeValue("delimiter", '\0');
    rx_worker.Configure();

    auto bind_success = rx_worker.Bind();
    ASSERT_TRUE(bind_success);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto tx_component = std::make_shared<Component>("tx_comp");
    Tcp_Worker tx_worker(*tx_component, "tx_worker");
    tx_worker.SetAttributeValue("ip_address", ip_address);
    tx_worker.SetAttributeValue("port", port);

    auto connect_success = tx_worker.Connect();
    EXPECT_TRUE(connect_success);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    int recd_bytes = 0;
    auto future = std::async(std::launch::async, [&recd_bytes, send_count, long_message, &rx_worker](){
        while(recd_bytes < send_count * long_message.size()) {

            TcpWorkerImpl::Message temp_msg;
            auto error = rx_worker.Receive(temp_msg);
            if(!error) {
                recd_bytes += temp_msg.size();
            } else {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });


    for(int i = 0; i < send_count; ++i) {
        std::vector<char> send_val(long_message.begin(), long_message.end());
        send_val.push_back('\0');
        tx_worker.Send(send_val);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    tx_worker.Disconnect();
    rx_worker.Disconnect();

    tx_worker.Terminate();
    rx_worker.Terminate();


    EXPECT_EQ(recd_bytes, send_count * long_message.size());

}
*/

// Test split message utility function.
// Should be able to take a message type and split it based on a delimiter.
// Any trailing content should be returned as an optional second val in the tuple
// Message type is currently std::vector<char>
TEST(Util, SplitMessage) {
    std::string test_message = "this is a test message.";
    std::vector<char> send_val(test_message.begin(), test_message.end());

    auto [messages, partial_message] = TcpWorkerImpl::SplitMessage(send_val, ' ');

    EXPECT_EQ(messages.size(), 4);
    EXPECT_EQ(partial_message.value().size(), 8);
}

// TODO: This test is currently failing in a CI setting.
// Passes just fine in any context other than jenkins
/*
TEST(Complex, BoundSend) {
     std::string long_message = "<test>";
    int send_count = 10;
    auto [ip_address, port] = get_test_endpoint();

    auto rx_component = std::make_shared<Component>("rx_comp");
    Tcp_Worker rx_worker(*rx_component, "rx_worker");
    rx_worker.SetAttributeValue("ip_address", ip_address);
    rx_worker.SetAttributeValue("port", port);
    rx_worker.SetAttributeValue("delimiter", '\0');
    rx_worker.Configure();

    int recd_bytes = 0;
    rx_worker.SetTcpCallback([&rx_worker, &recd_bytes, &long_message](std::vector<char> data) {
        recd_bytes += data.size();
        std::string out(data.begin(), data.end());
        ASSERT_EQ(rx_worker.Send(data), data.size());
        EXPECT_EQ(out, long_message);
    });

    auto bind_success = rx_worker.Bind();
    ASSERT_TRUE(bind_success);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto tx_component = std::make_shared<Component>("tx_comp");
    Tcp_Worker tx_worker(*tx_component, "tx_worker");
    tx_worker.SetAttributeValue("ip_address", ip_address);
    tx_worker.SetAttributeValue("port", port);

    auto connect_success = tx_worker.Connect();
    EXPECT_TRUE(connect_success);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    for(int i = 0; i < send_count; ++i) {
        std::vector<char> send_val(long_message.begin(), long_message.end());
        send_val.push_back('\0');
        tx_worker.Send(send_val);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    tx_worker.Disconnect();
    rx_worker.Disconnect();

    EXPECT_EQ(recd_bytes, send_count * long_message.size());

    rx_worker.Terminate();
    tx_worker.Terminate();
}
 */