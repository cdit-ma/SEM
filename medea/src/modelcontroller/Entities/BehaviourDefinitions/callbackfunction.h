#ifndef MEDEA_CALLBACKFUNCTION_H
#define MEDEA_CALLBACKFUNCTION_H

#include "../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class CallbackFunction : public Node{
        friend class ::EntityFactory;
        
        public:
            static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
        protected:
            CallbackFunction(EntityFactoryBroker& factory, bool is_temp_node);
            QSet<Node*> getDependants() const;
            void parentSet(Node* parent);
        public:
            bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_CALLBACKFUNCTION_H
