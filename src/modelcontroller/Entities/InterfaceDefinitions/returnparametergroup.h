#ifndef MEDEA_RETURNPARAMETERGROUP_H
#define MEDEA_RETURNPARAMETERGROUP_H
#include "../node.h"

class EntityFactory;
namespace MEDEA{
    class ReturnParameterGroup : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        static void RegisterWithEntityFactory(EntityFactory& factory);
        ReturnParameterGroup(EntityFactory& factory, bool is_temp_node);
        void parentSet(Node* parent);
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_RETURNPARAMETERGROUP_H
