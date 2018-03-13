#ifndef DEPLOYMENTRULE_H
#define DEPLOYMENTRULE_H

class DeploymentRule{
    public:

        
        virtual ~DeploymentRule() {};

    protected:
        DeploymentRule& operator=(const DeploymentRule&) = default;

};

#endif //DEPLOYMENTRULE_H