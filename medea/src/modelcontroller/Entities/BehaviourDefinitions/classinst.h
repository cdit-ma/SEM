#ifndef MEDEA_CLASS_INST_H
#define MEDEA_CLASS_INST_H
#include "../node.h"

class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ClassInst : public Node{
        friend class ::EntityFactory;
        
        public:
            static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
        protected:
            ClassInst(EntityFactoryBroker& factory, bool is_temp_node);
            void parentSet(Node* parent) override;
            bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst) override;
    };
};

#endif // MEDEA_CLASS_INST_H
