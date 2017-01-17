#ifndef DEPLOYMENT_4_H
#define DEPLOYMENT_4_H

#include "common/includes/core/nodecontainer.h"

class Deployment_4: public NodeContainer{
    public:
        void startup();
};

extern "C" NodeContainer* create_deployment()
{
  return new Deployment_4();
};

extern "C" void destroy_deployment(NodeContainer* object)
{ 
    Deployment_4* instance = static_cast<Deployment_4*>(object);
    if(instance){
        delete instance;
    }
};
#endif