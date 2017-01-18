#ifndef DEPLOYMENT_10_H
#define DEPLOYMENT_10_H

#include "common/includes/core/nodecontainer.h"

class Deployment_10: public NodeContainer{
    public:
        void startup();
};

extern "C" NodeContainer* create_deployment()
{
  return new Deployment_10();
};

extern "C" void destroy_deployment(NodeContainer* object)
{ 
    Deployment_10* instance = static_cast<Deployment_10*>(object);
    if(instance){
        delete instance;
    }
};
#endif