#ifndef DEPLOYMENT_2_H
#define DEPLOYMENT_2_H

#include <core/nodecontainer.h>

class Deployment_2: public NodeContainer{
    public:
        Deployment_2();
        void startup();
        //::EventPort* construct_rx(std::string middleware, std::string datatype, Component* component, std::string port_name);
        //::EventPort* construct_tx(std::string middleware, std::string datatype, Component* component, std::string port_name);
        //::Component* construct_component(std::string componentType, std::string component_name);
};

extern "C" NodeContainer* create_deployment()
{
    std::cout << "entry point" << std::endl;
    return new Deployment_2();
};

extern "C" void destroy_deployment(NodeContainer* object)
{ 
    Deployment_2* instance = static_cast<Deployment_2*>(object);
    if(instance){
        delete instance;
    }
};
#endif