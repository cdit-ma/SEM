#ifndef MEDEA_INPUTPARAMETERGROUP_INSTANCE_H
#define MEDEA_INPUTPARAMETERGROUP_INSTANCE_H
#include "../node.h"

class EntityFactory;
namespace MEDEA{
    class InputParameterGroupInstance : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        InputParameterGroupInstance(EntityFactory* factory);
        InputParameterGroupInstance();
    };
};

#endif // MEDEA_INPUTPARAMETERGROUP_INSTANCE_H
