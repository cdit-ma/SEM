#ifndef MEDEA_RETURNPARAMETERGROUP_INSTANCE_H
#define MEDEA_RETURNPARAMETERGROUP_INSTANCE_H
#include "../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ReturnParameterGroupInstance : public Node{

   

    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        ReturnParameterGroupInstance(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_RETURNPARAMETERGROUP_INSTANCE_H
