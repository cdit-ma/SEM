#ifndef DEPLOYMENT_3_H
#define DEPLOYMENT_3_H

#include "common/includes/core/nodecontainer.h"


class Deployment_3: public NodeContainer{
    public:
        void startup();
};

extern "C" NodeContainer* create_deployment()
{
  return new Deployment_3();
};

extern "C" void destroy_deployment(NodeContainer* object)
{ 
    Deployment_3* instance = static_cast<Deployment_3*>(object);
    if(instance){
        delete instance;
    }
};
#endif