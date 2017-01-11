#ifndef DEPLOYMENT_1_H
#define DEPLOYMENT_1_H

#include "common/includes/globalinterfaces.hpp"
class Deployment_1: public NodeContainer{
    public:
        void startup();
};

extern "C" NodeContainer* create_object()
{
  return new Deployment_1();
};

extern "C" void destroy_object(NodeContainer* object)
{ 
    Deployment_1* instance = static_cast<Deployment_1*>(object);
    if(instance){
        delete instance;
    }
};
#endif