#ifndef MEDEA_ELSE_CONDITION_H
#define MEDEA_ELSE_CONDITION_H
#include "../containernode.h"
#include "../../InterfaceDefinitions/datanode.h"

class EntityFactory;
namespace MEDEA{
    class ElseCondition: public DataNode, public ContainerNode
    {
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ElseCondition(EntityFactory* factory);
        ElseCondition();
    };
};

#endif // MEDEA_ELSE_CONDITION_H
