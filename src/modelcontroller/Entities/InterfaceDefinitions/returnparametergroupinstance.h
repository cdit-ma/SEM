#ifndef MEDEA_RETURNPARAMETERGROUP_INSTANCE_H
#define MEDEA_RETURNPARAMETERGROUP_INSTANCE_H
#include "../node.h"

class EntityFactory;
namespace MEDEA{
    class ReturnParameterGroupInstance : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ReturnParameterGroupInstance(EntityFactory* factory);
        ReturnParameterGroupInstance();
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_RETURNPARAMETERGROUP_INSTANCE_H
