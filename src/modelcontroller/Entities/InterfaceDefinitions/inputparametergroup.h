#ifndef MEDEA_INPUTPARAMETERGROUP_H
#define MEDEA_INPUTPARAMETERGROUP_H
#include "../node.h"

class EntityFactory;
namespace MEDEA{
    class InputParameterGroup : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        InputParameterGroup(EntityFactory* factory);
        InputParameterGroup();
        void parentSet(Node* parent) override;
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_INPUTPARAMETERGROUP_H
