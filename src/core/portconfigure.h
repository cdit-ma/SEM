#ifndef CORE_PORTCONFIGURE_H
#define CORE_PORTCONFIGURE_H

#include "eventport.h"

namespace qpid{
    void ConfigureInEventPort(EventPort* port, std::string broker, std::string topic);
    void ConfigureOutEventPort(EventPort* port, std::string broker, std::string topic);
}

namespace rti{
    void ConfigureInEventPort(EventPort* port, int domain, std::string subscriber_name, std::string topic_name);
    void ConfigureOutEventPort(EventPort* port, int domain, std::string subscriber_name, std::string topic_name);
}

namespace ospl{
    void ConfigureInEventPort(EventPort* port, int domain, std::string subscriber_name, std::string topic_name);
    void ConfigureOutEventPort(EventPort* port, int domain, std::string subscriber_name, std::string topic_name);
}

namespace zmq{
    void ConfigureInEventPort(EventPort* port, std::vector<std::string> end_points);
    void ConfigureOutEventPort(EventPort* port, std::string end_point);
}

#endif //CORE_PORTCONFIGURE_H
