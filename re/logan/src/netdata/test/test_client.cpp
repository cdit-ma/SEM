//
// Created by Jackson on 16/3/21.
//

#include "netdata/tcp_receiver.h"

#include "netdata/chart_json_parser.hpp"

#include "netdata/device_sample_aggregator.hpp"

namespace sem::logging::netdata::test {

    /**
     * Simple printer class for prototyping/testing tcp stream functionality
     */
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
    auto parser = std::make_shared<chart_json_parser>();
    auto converter = std::make_shared<device_sample_aggregator>();

    parser->register_listener(converter, protobuf::chart::nvidia_smi);

    auto receiver = std::make_shared<tcp_receiver>(3456);

    receiver->register_stream_listener(parser);

    std::thread consumer_thread([converter](){
        while (true) {
            auto metrics = converter->retrieve_device_metrics("v100_centos_7");
            if (metrics) {
                std::cout << metrics->DebugString() << std::endl;
            }
            sleep(1);
        }
    });

    receiver->wait();

    return 0;
}