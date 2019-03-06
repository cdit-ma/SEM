#ifndef MEDEA_CLASS_H
#define MEDEA_CLASS_H
#include "../node.h"

class EntityFactoryRegistryBroker;
namespace MEDEA{
    class Class : public Node{
        friend class ::EntityFactory;
        public:
            static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
        protected:
            Class(EntityFactoryBroker& factory, bool is_temp_node);
            void parentSet(Node* parent) override;
    };
};

#endif // MEDEA_CLASS_H
