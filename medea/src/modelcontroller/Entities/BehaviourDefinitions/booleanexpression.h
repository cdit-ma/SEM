#ifndef MEDEA_BOOLEANEXPRESSION_H
#define MEDEA_BOOLEANEXPRESSION_H
#include "../node.h"
#include "../InterfaceDefinitions/datanode.h"



class EntityFactoryRegistryBroker;
namespace MEDEA{
    class BooleanExpression: public ::DataNode
    {
   
    friend class ::EntityFactory;
    public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
    protected:
        BooleanExpression(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node* child);

        DataNode* GetLhs();
        DataNode* GetRhs();
        DataNode* GetComparator();
    private:
        void updateLabel();
        DataNode* lhs_ = 0;
        DataNode* comparator_ = 0;
        DataNode* rhs_ = 0;
    };
}

#endif //MEDEA_BOOLEANEXPRESSION_H
