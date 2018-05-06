#ifndef MEDEA_BOOLEANEXPRESSION_H
#define MEDEA_BOOLEANEXPRESSION_H
#include "../node.h"
#include "../InterfaceDefinitions/datanode.h"

class EntityFactory;

namespace MEDEA{
    class BooleanExpression: public ::DataNode
    {
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        static Node* ConstructBooleanExpression(EntityFactory* factory);
        BooleanExpression(EntityFactory* factory);
        BooleanExpression();
    
        void updateLabel();
        void updateOutputType();
    public:
        bool canAdoptChild(Node* child);
    };
}

#endif //MEDEA_BOOLEANEXPRESSION_H
