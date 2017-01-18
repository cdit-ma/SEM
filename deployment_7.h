#ifndef DEPLOYMENT_7_H
#define DEPLOYMENT_7_H

#include "common/includes/core/nodecontainer.h"


class Deployment_7: public NodeContainer{
    public:
        void startup();
};

extern "C" NodeContainer* create_deployment()
{
  return new Deployment_7();
};

extern "C" void destroy_deployment(NodeContainer* object)
{ 
    Deployment_7* instance = static_cast<Deployment_7*>(object);
    if(instance){
        delete instance;
    }
};
#endif