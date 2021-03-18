//
// Created by Jackson Michael on 5/3/21.
//

#ifndef SEM_NETDATA_CHART_LISTENER_HPP
#define SEM_NETDATA_CHART_LISTENER_HPP

namespace sem::logging::netdata {

    class chart_listener {
    public:
        virtual ~chart_listener() = 0;
    };

    inline chart_listener::~chart_listener() = default;
}

#endif //SEM_NETDATA_CHART_LISTENER_HPP
