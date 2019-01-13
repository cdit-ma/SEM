
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
        public:
            bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_CALLBACKFUNCTION_INST_H
