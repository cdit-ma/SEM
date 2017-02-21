#include "portconfigure.h"

void qpid::ConfigureInEventPort(EventPort* port, std::string broker, std::string topic){
    if(!port){
        return;
    }

    std::map<std::string, ::Attribute*> attrs;

    Attribute* broker_attr = new Attribute(ATTRIBUTE_TYPE::STRING, "broker");
    broker_attr->set_String(broker);
    attrs["broker"] = broker_attr;

    Attribute* topic_attr = new Attribute(ATTRIBUTE_TYPE::STRING, "topic_name");
    topic_attr->set_String(topic);
    attrs["topic_name"] = topic_attr;

    port->Startup(attrs);
}

void qpid::ConfigureOutEventPort(EventPort* port, std::string broker, std::string topic){
    if(!port){
        return;
    }
    std::map<std::string, ::Attribute*> attrs;
    
    Attribute* broker_attr = new Attribute(ATTRIBUTE_TYPE::STRING, "broker");
    broker_attr->set_String(broker);
    attrs["broker"] = broker_attr;

    
    Attribute* topic_attr = new Attribute(ATTRIBUTE_TYPE::STRING, "topic_name");
    topic_attr->set_String(topic);
    attrs["topic_name"] = topic_attr;
    
    port->Startup(attrs);
}

void rti::ConfigureInEventPort(EventPort* port, int domain, std::string subscriber_name, std::string topic_name){
    if(!port){
        return;
    }
    std::map<std::string, ::Attribute*> attrs;
    
    Attribute* sub_attr = new Attribute(ATTRIBUTE_TYPE::STRING, "subscriber_name");
    sub_attr->set_String(subscriber_name);
    attrs["subscriber_name"] = sub_attr;

    
    Attribute* topic_attr = new Attribute(ATTRIBUTE_TYPE::STRING, "topic_name");
    topic_attr->set_String(topic_name);
    attrs["topic_name"] = topic_attr;

    Attribute* domain_attr = new Attribute(ATTRIBUTE_TYPE::INTEGER, "domain_id");
    domain_attr->set_Integer(domain);
    attrs["domain_id"] = domain_attr;
    
    port->Startup(attrs);
}


void rti::ConfigureOutEventPort(EventPort* port, int domain, std::string publisher_name, std::string topic_name){
    if(!port){
        return;
    }
    std::map<std::string, ::Attribute*> attrs;
    
    Attribute* pub_attr = new Attribute(ATTRIBUTE_TYPE::STRING, "publisher_name");
    pub_attr->set_String(publisher_name);
    attrs["publisher_name"] = pub_attr;

    
    Attribute* topic_attr = new Attribute(ATTRIBUTE_TYPE::STRING, "topic_name");
    topic_attr->set_String(topic_name);
    attrs["topic_name"] = topic_attr;

    Attribute* domain_attr = new Attribute(ATTRIBUTE_TYPE::INTEGER, "domain_id");
    domain_attr->set_Integer(domain);
    attrs["domain_id"] = domain_attr;
    
    port->Startup(attrs);
}

void ospl::ConfigureInEventPort(EventPort* port, int domain, std::string subscriber_name, std::string topic_name){
    if(!port){
        return;
    }
    std::map<std::string, ::Attribute*> attrs;
    
    Attribute* sub_attr = new Attribute(ATTRIBUTE_TYPE::STRING, "subscriber_name");
    sub_attr->set_String(subscriber_name);
    attrs["subscriber_name"] = sub_attr;

    
    Attribute* topic_attr = new Attribute(ATTRIBUTE_TYPE::STRING, "topic_name");
    topic_attr->set_String(topic_name);
    attrs["topic_name"] = topic_attr;

    Attribute* domain_attr = new Attribute(ATTRIBUTE_TYPE::INTEGER, "domain_id");
    domain_attr->set_Integer(domain);
    attrs["domain_id"] = domain_attr;
    
    port->Startup(attrs);
}


void ospl::ConfigureOutEventPort(EventPort* port, int domain, std::string publisher_name, std::string topic_name){
    if(!port){
        return;
    }
    std::map<std::string, ::Attribute*> attrs;
    
    Attribute* pub_attr = new Attribute(ATTRIBUTE_TYPE::STRING, "publisher_name");
    pub_attr->set_String(publisher_name);
    attrs["publisher_name"] = pub_attr;

    
    Attribute* topic_attr = new Attribute(ATTRIBUTE_TYPE::STRING, "topic_name");
    topic_attr->set_String(topic_name);
    attrs["topic_name"] = topic_attr;

    Attribute* domain_attr = new Attribute(ATTRIBUTE_TYPE::INTEGER, "domain_id");
    domain_attr->set_Integer(domain);
    attrs["domain_id"] = domain_attr;
    
    port->Startup(attrs);
}

void zmq::ConfigureInEventPort(EventPort* port, std::vector<std::string> end_points){
    if(!port){
        return;
    }
    std::map<std::string, ::Attribute*> attrs;

    Attribute* pub_attr = new Attribute(ATTRIBUTE_TYPE::STRING, "publisher_address");

    for(auto str : end_points){
        pub_attr->StringList().push_back(str);
    }

    attrs["publisher_address"] = pub_attr;
    port->Startup(attrs);
}

void zmq::ConfigureOutEventPort(EventPort* port, std::string end_point){
    if(!port){
        return;
    }
    std::map<std::string, ::Attribute*> attrs;

    Attribute* attr = new Attribute(ATTRIBUTE_TYPE::STRING, "publisher_address");
    attr->set_String(end_point);

    attrs["publisher_address"] = attr;
    port->Startup(attrs);
}
