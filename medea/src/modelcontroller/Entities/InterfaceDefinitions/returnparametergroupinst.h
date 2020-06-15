#ifndef MEDEA_RETURNPARAMETERGROUP_INST_H
#define MEDEA_RETURNPARAMETERGROUP_INST_H
#include "../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ReturnParameterGroupInst : public Node{
        public:
            static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
        protected:
            ReturnParameterGroupInst(EntityFactoryBroker& factory, bool is_temp_node);
        public:
            bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_RETURNPARAMETERGROUP_INST_H
