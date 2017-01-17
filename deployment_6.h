#ifndef DEPLOYMENT_6_H
#define DEPLOYMENT_6_H

#include "common/includes/core/nodecontainer.h"

class Deployment_6: public NodeContainer{
    public:
        void startup();
};

extern "C" NodeContainer* create_deployment()
{
  return new Deployment_6();
};

extern "C" void destroy_deployment(NodeContainer* object)
{ 
    Deployment_6* instance = static_cast<Deployment_6*>(object);
    if(instance){
        delete instance;
    }
};
#endif