//
// Created by Jackson Michael on 5/3/21.
//

#ifndef SEM_NETDATA_JSON_INFO_HPP
#define SEM_NETDATA_JSON_INFO_HPP

#include <string>

namespace sem::logging::netdata::json {


    enum class chart_type {
        nvidia_smi
    };

    template <chart_type ChartType>
    struct info {
        static_assert("No information provided for instantiated chart type");
    };

    template <>
    struct info<chart_type::nvidia_smi> {
        std::string type_name = "nvidia_smi";
    };
}

#endif //SEM_NETDATA_JSON_INFO_HPP
