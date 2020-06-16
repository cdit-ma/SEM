#ifndef MEDEA_INPUTPARAMETERGROUP_H
#define MEDEA_INPUTPARAMETERGROUP_H
#include "../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class InputParameterGroup : public Node{
   
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        InputParameterGroup(EntityFactoryBroker& factory, bool is_temp_node);
        void parentSet(Node* parent);
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_INPUTPARAMETERGROUP_H
