#ifndef MEDEA_RETURNPARAMETERGROUP_H
#define MEDEA_RETURNPARAMETERGROUP_H
#include "../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ReturnParameterGroup : public Node{

   
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        ReturnParameterGroup(EntityFactoryBroker& factory, bool is_temp_node);
        void parentSet(Node* parent);
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_RETURNPARAMETERGROUP_H
