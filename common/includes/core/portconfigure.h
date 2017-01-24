#ifndef CORE_PORTCONFIGURE_H
#define CORE_PORTCONFIGURE_H

namespace qpid{
    static void configure_in_event_port(EventPort* port, std::string broker, std::string topic);
    static void configure_out_event_port(EventPort* port, std::string broker, std::string topic);
}

namespace rti{
    static void configure_in_event_port(EventPort* port, int domain, std::string subscriber_name, std::string topic_name);
    static void configure_out_event_port(EventPort* port, int domain, std::string subscriber_name, std::string topic_name);
}

namespace ospl{
    static void configure_in_event_port(EventPort* port, int domain, std::string subscriber_name, std::string topic_name);
    static void configure_out_event_port(EventPort* port, int domain, std::string subscriber_name, std::string topic_name);
}

namespace zmq{
    static void configure_in_event_port(EventPort* port, std::vector<std::string> end_points);
    static void configure_out_event_port(EventPort* port, std::string end_point);
}

#endif //CORE_PORTCONFIGURE_H
