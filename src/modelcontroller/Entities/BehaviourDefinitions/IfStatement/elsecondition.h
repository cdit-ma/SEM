#ifndef MEDEA_ELSE_CONDITION_H
#define MEDEA_ELSE_CONDITION_H
#include "../../InterfaceDefinitions/datanode.h"

class EntityFactory;
namespace MEDEA{
    class ElseCondition: public DataNode{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ElseCondition(EntityFactory* factory);
        ElseCondition();
    };
};

#endif // MEDEA_ELSE_CONDITION_H
