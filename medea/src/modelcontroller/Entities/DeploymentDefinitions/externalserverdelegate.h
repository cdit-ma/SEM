#ifndef MEDEA_EXTERNALSERVERDELEGATE_H
#define MEDEA_EXTERNALSERVERDELEGATE_H

#include "eventportassembly.h"
class EntityFactoryRegistryBroker;

namespace MEDEA{
    class ExternalServerDelegate : public EventPortAssembly
    {
        Q_OBJECT
        public:
            static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
        protected:
            ExternalServerDelegate(EntityFactoryBroker& factory, bool is_temp_node);
        public:
            bool canAcceptEdge(EDGE_KIND edge_kind, Node *dst);
            bool canAdoptChild(Node* child);
        private:
            void MiddlewareUpdated();
            Node* in_ = 0;
    };
};

#endif // MEDEA_EXTERNALSERVERDELEGATE_H
