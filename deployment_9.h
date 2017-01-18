#ifndef DEPLOYMENT_9_H
#define DEPLOYMENT_9_H

#include "common/includes/core/nodecontainer.h"


class Deployment_9: public NodeContainer{
    public:
        void startup();
};

extern "C" NodeContainer* create_deployment()
{
  return new Deployment_9();
};

extern "C" void destroy_deployment(NodeContainer* object)
{ 
    Deployment_9* instance = static_cast<Deployment_9*>(object);
    if(instance){
        delete instance;
    }
};
#endif