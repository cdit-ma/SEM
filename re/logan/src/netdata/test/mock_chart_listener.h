//
// Created by Jackson Michael on 27/3/21.
//

#ifndef SEM_MOCK_CHART_LISTENER_H
#define SEM_MOCK_CHART_LISTENER_H

#include "gmock/gmock.h"
#include "netdata/chart_listener.hpp"

namespace sem::logging::netdata::test {

    class mock_chart_listener : public chart_listener {
    public:
        mock_chart_listener() = default;
        ~mock_chart_listener() override = default;

        MOCK_METHOD(void, receive_chart_data, (std::shared_ptr<const protobuf::chart>), (override));
    };
}


#endif //SEM_MOCK_CHART_LISTENER_H
