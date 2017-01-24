#ifndef CORE_PORTCONFIGURE_H
#define CORE_PORTCONFIGURE_H

#include "eventport.h"

namespace qpid{
    void configure_in_event_port(EventPort* port, std::string broker, std::string topic);
    void configure_out_event_port(EventPort* port, std::string broker, std::string topic);
}

namespace rti{
    void configure_in_event_port(EventPort* port, int domain, std::string subscriber_name, std::string topic_name);
    void configure_out_event_port(EventPort* port, int domain, std::string subscriber_name, std::string topic_name);
}

namespace ospl{
    void configure_in_event_port(EventPort* port, int domain, std::string subscriber_name, std::string topic_name);
    void configure_out_event_port(EventPort* port, int domain, std::string subscriber_name, std::string topic_name);
}

namespace zmq{
    void configure_in_event_port(EventPort* port, std::vector<std::string> end_points);
    void configure_out_event_port(EventPort* port, std::string end_point);
}

#endif //CORE_PORTCONFIGURE_H
