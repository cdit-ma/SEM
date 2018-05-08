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
        static void RegisterWithEntityFactory(EntityFactory& factory);
        BooleanExpression(EntityFactory& factory, bool is_temp_node);
        
    public:
        bool canAdoptChild(Node* child);
    private:
        void updateLabel();
        Node* lhs_ = 0;
        Node* comparator_ = 0;
        Node* rhs_ = 0;
    };
}

#endif //MEDEA_BOOLEANEXPRESSION_H
