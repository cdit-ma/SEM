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
        static void RegisterWithEntityFactory(EntityFactory& factory);
        IfCondition(EntityFactory& factory, bool is_temp_node);
    };
};

#endif // MEDEA_IF_CONDITION_H
