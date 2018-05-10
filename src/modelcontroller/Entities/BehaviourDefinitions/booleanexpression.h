#ifndef MEDEA_BOOLEANEXPRESSION_H
#define MEDEA_BOOLEANEXPRESSION_H
#include "../node.h"
#include "../InterfaceDefinitions/datanode.h"



class EntityFactoryRegistryBroker;
namespace MEDEA{
    class BooleanExpression: public ::DataNode
    {
   
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        BooleanExpression(EntityFactoryBroker& factory, bool is_temp_node);
        
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
