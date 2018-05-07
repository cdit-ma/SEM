#ifndef MEDEA_RETURNPARAMETERGROUP_H
#define MEDEA_RETURNPARAMETERGROUP_H
#include "../node.h"

class EntityFactory;
namespace MEDEA{
    class ReturnParameterGroup : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ReturnParameterGroup(EntityFactory* factory);
        ReturnParameterGroup();
        void parentSet(Node* parent);
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_RETURNPARAMETERGROUP_H
