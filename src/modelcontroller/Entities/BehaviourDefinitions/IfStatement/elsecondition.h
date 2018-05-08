#ifndef MEDEA_ELSE_CONDITION_H
#define MEDEA_ELSE_CONDITION_H
#include "../../InterfaceDefinitions/datanode.h"

class EntityFactory;
namespace MEDEA{
    class ElseCondition: public DataNode{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        static void RegisterWithEntityFactory(EntityFactory& factory);
        ElseCondition(EntityFactory& factory, bool is_temp_node);
    };
};

#endif // MEDEA_ELSE_CONDITION_H
