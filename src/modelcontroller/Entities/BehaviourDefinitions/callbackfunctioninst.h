
#ifndef MEDEA_CALLBACKFUNCTION_INST_H
#define MEDEA_CALLBACKFUNCTION_INST_H

#include "../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class CallbackFunctionInst : public Node{
        friend class ::EntityFactory;
        
        public:
            static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
        protected:
            CallbackFunctionInst(EntityFactoryBroker& factory, bool is_temp_node);
            QSet<Node*> getListOfValidAncestorsForChildrenDefinitions();
	        QSet<Node*> getParentNodesForValidDefinition();
        public:
            bool canAdoptChild(Node* child);
        private:
            Node* getTopBehaviourContainer();
            bool top_behaviour_calculated = false;
            Node* top_behaviour_container = 0;
    };
};

#endif // MEDEA_CALLBACKFUNCTION_INST_H
