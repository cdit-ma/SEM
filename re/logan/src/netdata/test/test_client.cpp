//
// Created by Jackson on 16/3/21.
//

#include "netdata/tcp_receiver.h"

namespace sem::logging::netdata::test {

    class print_listener : public tcp_stream_listener {
    public:
        ~print_listener() override = default;

        void receive_line(std::string tcp_stream_line) override {
            std::cout << tcp_stream_line << std::endl;
        }
    };
}


using namespace sem::logging::netdata;

int main(int argc, char** argv) {
    auto printer = std::make_shared<test::print_listener>();
    auto receiver = std::make_shared<tcp_receiver>(3456);

    receiver->register_stream_listener(printer);

    receiver->wait();
}