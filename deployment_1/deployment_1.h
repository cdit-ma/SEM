#ifndef DEPLOYMENT_1_H
#define DEPLOYMENT_1_H

#include "common/includes/core/nodecontainer.h"


class Deployment_1: public NodeContainer{
    public:
        void startup();
};

extern "C" NodeContainer* create_deployment()
{
  return new Deployment_1();
};

extern "C" void destroy_deployment(NodeContainer* object)
{ 
    Deployment_1* instance = static_cast<Deployment_1*>(object);
    if(instance){
        delete instance;
    }
};
#endif