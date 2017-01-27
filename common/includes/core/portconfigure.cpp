#include "portconfigure.h"

void qpid::configure_in_event_port(EventPort* port, std::string broker, std::string topic){
    if(!port){
        return;
    }

    std::map<std::string, ::Attribute*> attrs;

    Attribute* broker_attr = new Attribute(AT_STRING, "broker");
    broker_attr->set_string(broker);
    attrs["broker"] = broker_attr;

    Attribute* topic_attr = new Attribute(AT_STRING, "topic_name");
    topic_attr->set_string(topic);
    attrs["topic_name"] = topic_attr;

    port->startup(attrs);
}

void qpid::configure_out_event_port(EventPort* port, std::string broker, std::string topic){
    if(!port){
        return;
    }
    std::map<std::string, ::Attribute*> attrs;
    
    Attribute* broker_attr = new Attribute(AT_STRING, "broker");
    broker_attr->set_string(broker);
    attrs["broker"] = broker_attr;

    
    Attribute* topic_attr = new Attribute(AT_STRING, "topic_name");
    topic_attr->set_string(topic);
    attrs["topic_name"] = topic_attr;
    
    port->startup(attrs);
}

void rti::configure_in_event_port(EventPort* port, int domain, std::string subscriber_name, std::string topic_name){
    if(!port){
        return;
    }
    std::map<std::string, ::Attribute*> attrs;
    
    Attribute* sub_attr = new Attribute(AT_STRING, "subscriber_name");
    sub_attr->set_string(subscriber_name);
    attrs["subscriber_name"] = sub_attr;

    
    Attribute* topic_attr = new Attribute(AT_STRING, "topic_name");
    topic_attr->set_string(topic_name);
    attrs["topic_name"] = topic_attr;

    Attribute* domain_attr = new Attribute(AT_INTEGER, "domain_id");
    domain_attr->i = domain;
    attrs["domain_id"] = domain_attr;
    
    port->startup(attrs);
}


void rti::configure_out_event_port(EventPort* port, int domain, std::string publisher_name, std::string topic_name){
    if(!port){
        return;
    }
    std::map<std::string, ::Attribute*> attrs;
    
    Attribute* pub_attr = new Attribute(AT_STRING, "publisher_name");
    pub_attr->set_string(publisher_name);
    attrs["publisher_name"] = pub_attr;

    
    Attribute* topic_attr = new Attribute(AT_STRING, "topic_name");
    topic_attr->set_string(topic_name);
    attrs["topic_name"] = topic_attr;

    Attribute* domain_attr = new Attribute(AT_INTEGER, "domain_id");
    domain_attr->i = domain;
    attrs["domain_id"] = domain_attr;
    
    port->startup(attrs);
}

void ospl::configure_in_event_port(EventPort* port, int domain, std::string subscriber_name, std::string topic_name){
    if(!port){
        return;
    }
    std::map<std::string, ::Attribute*> attrs;
    
    Attribute* sub_attr = new Attribute(AT_STRING, "subscriber_name");
    sub_attr->set_string(subscriber_name);
    attrs["subscriber_name"] = sub_attr;

    
    Attribute* topic_attr = new Attribute(AT_STRING, "topic_name");
    topic_attr->set_string(topic_name);
    attrs["topic_name"] = topic_attr;

    Attribute* domain_attr = new Attribute(AT_INTEGER, "domain_id");
    domain_attr->i = domain;
    attrs["domain_id"] = domain_attr;
    
    port->startup(attrs);
}


void ospl::configure_out_event_port(EventPort* port, int domain, std::string publisher_name, std::string topic_name){
    if(!port){
        return;
    }
    std::map<std::string, ::Attribute*> attrs;
    
    Attribute* pub_attr = new Attribute(AT_STRING, "publisher_name");
    pub_attr->set_string(publisher_name);
    attrs["publisher_name"] = pub_attr;

    
    Attribute* topic_attr = new Attribute(AT_STRING, "topic_name");
    topic_attr->set_string(topic_name);
    attrs["topic_name"] = topic_attr;

    Attribute* domain_attr = new Attribute(AT_INTEGER, "domain_id");
    domain_attr->i = domain;
    attrs["domain_id"] = domain_attr;
    
    port->startup(attrs);
}

void zmq::configure_in_event_port(EventPort* port, std::vector<std::string> end_points){
    if(!port){
        return;
    }
    std::map<std::string, ::Attribute*> attrs;

    Attribute* pub_attr = new Attribute(AT_STRING, "publisher_address");

    for(auto str : end_points){
        pub_attr->s.push_back(str);
    }

    attrs["publisher_address"] = pub_attr;
    port->startup(attrs);
}

void zmq::configure_out_event_port(EventPort* port, std::string end_point){
    if(!port){
        return;
    }
    std::map<std::string, ::Attribute*> attrs;

    Attribute* attr = new Attribute(AT_STRING, "publisher_address");
    attr->set_string(end_point);

    attrs["publisher_address"] = attr;
    port->startup(attrs);
}