#ifndef DEPLOYMENT_8_H
#define DEPLOYMENT_8_H

#include "common/includes/core/nodecontainer.h"

class Deployment_8: public NodeContainer{
    public:
        void startup();
};

extern "C" NodeContainer* create_deployment()
{
  return new Deployment_8();
};

extern "C" void destroy_deployment(NodeContainer* object)
{ 
    Deployment_8* instance = static_cast<Deployment_8*>(object);
    if(instance){
        delete instance;
    }
};
#endif