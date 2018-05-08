#ifndef MEDEA_INPUTPARAMETERGROUP_INSTANCE_H
#define MEDEA_INPUTPARAMETERGROUP_INSTANCE_H
#include "../node.h"

class EntityFactory;
namespace MEDEA{
    class InputParameterGroupInstance : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        static void RegisterWithEntityFactory(EntityFactory& factory);
        InputParameterGroupInstance(EntityFactory& factory, bool is_temp_node);
    };
};

#endif // MEDEA_INPUTPARAMETERGROUP_INSTANCE_H
