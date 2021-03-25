//
// Created by Jackson Michael on 5/3/21.
//

#ifndef SEM_NETDATA_CHART_LISTENER_HPP
#define SEM_NETDATA_CHART_LISTENER_HPP

#include "netdata_chart.pb.h"

namespace sem::logging::netdata {

    class chart_listener {
    public:
        virtual ~chart_listener() = 0;

        /**
         * Classes implementing this interface much override this function to handle the netdata charts
         * they are registered to listen for.
         * @param chart_data
         */
        virtual void receive_chart_data(std::shared_ptr<const protobuf::chart> chart_data) = 0;
    };

    inline chart_listener::~chart_listener() = default;
}

#endif //SEM_NETDATA_CHART_LISTENER_HPP
