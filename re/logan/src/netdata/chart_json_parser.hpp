//
// Created by Jackson Michael on 19/3/21.
//

#ifndef SEM_CHART_JSON_PARSER_HPP
#define SEM_CHART_JSON_PARSER_HPP

#include <string>
#include <unordered_map>

#include "tcp_stream_listener.hpp"
#include "chart_listener.hpp"

#include "google/protobuf/util/json_util.h"

#include "netdata_chart.pb.h"

#include <iostream>

namespace sem::logging::netdata {

    class chart_json_parser : public tcp_stream_listener {

    public:
        /**
         * Processes a line of JSON text, attempting to deserialize it as a netdata chart message
         * @param tcp_stream_line
         */
        void receive_line(std::string tcp_stream_line) override {
            auto chart = std::make_shared<protobuf::chart>();

            auto parse_status = google::protobuf::util::JsonStringToMessage(tcp_stream_line, chart.get());

            if (!parse_status.ok()) {
                std::cerr << "unable to parse nvidia_smi using JSON stream:\n" << parse_status.ToString() << std::endl;
            }

            auto type = chart->chart_type();

            try {
                for (const auto &listener : listeners_.at(type)) {
                    if (auto locked_ref = listener.lock()) {
                        locked_ref->receive_chart_data(chart);
                    }
                }
            } catch (const std::exception& ex) {

            }
        }

        /**
         *
         * @param listener
         * @param type
         */
        void register_listener(const std::weak_ptr<chart_listener> &listener, protobuf::chart::chart_type_enum type) {
            listeners_[type].push_back(listener);
        }

    private:
        std::unordered_map<protobuf::chart::chart_type_enum, std::vector<std::weak_ptr<chart_listener>>> listeners_;

    };
}

#endif //SEM_CHART_JSON_PARSER_HPP
