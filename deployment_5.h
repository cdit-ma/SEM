#ifndef DEPLOYMENT_5_H
#define DEPLOYMENT_5_H

#include "common/includes/core/nodecontainer.h"


class Deployment_5: public NodeContainer{
    public:
        void startup();
};

extern "C" NodeContainer* create_deployment()
{
  return new Deployment_5();
};

extern "C" void destroy_deployment(NodeContainer* object)
{ 
    Deployment_5* instance = static_cast<Deployment_5*>(object);
    if(instance){
        delete instance;
    }
};
#endif