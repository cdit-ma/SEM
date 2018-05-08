#ifndef MEDEA_RETURNPARAMETERGROUP_INSTANCE_H
#define MEDEA_RETURNPARAMETERGROUP_INSTANCE_H
#include "../node.h"

class EntityFactory;
namespace MEDEA{
    class ReturnParameterGroupInstance : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        static void RegisterWithEntityFactory(EntityFactory& factory);
        ReturnParameterGroupInstance(EntityFactory& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_RETURNPARAMETERGROUP_INSTANCE_H
