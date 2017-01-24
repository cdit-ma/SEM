#ifndef DEPLOYMENT_2_H
#define DEPLOYMENT_2_H

#include <core/nodecontainer.h>

class Deployment_2: public NodeContainer{
    public:
        void startup();
};

extern "C" NodeContainer* create_deployment()
{
  return new Deployment_2();
};

extern "C" void destroy_deployment(NodeContainer* object)
{ 
    Deployment_2* instance = static_cast<Deployment_2*>(object);
    if(instance){
        delete instance;
    }
};
#endif