#ifndef MEDEA_ELSEIF_CONDITION_H
#define MEDEA_ELSEIF_CONDITION_H
#include "../containernode.h"
#include "../../InterfaceDefinitions/datanode.h"


class EntityFactory;
namespace MEDEA{
    class ElseIfCondition: public DataNode{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ElseIfCondition(EntityFactory* factory);
        ElseIfCondition();
    };
};

#endif // MEDEA_ELSEIF_CONDITION_H
