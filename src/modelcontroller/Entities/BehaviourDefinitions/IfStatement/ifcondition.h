#ifndef MEDEA_IF_CONDITION_H
#define MEDEA_IF_CONDITION_H
#include "../containernode.h"
#include "../../InterfaceDefinitions/datanode.h"

class EntityFactory;
namespace MEDEA{
    class IfCondition: public DataNode
    {
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        IfCondition(EntityFactory* factory);
        IfCondition();
    };
};

#endif // MEDEA_IF_CONDITION_H
