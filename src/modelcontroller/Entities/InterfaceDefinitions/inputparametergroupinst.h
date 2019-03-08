#ifndef MEDEA_INPUTPARAMETERGROUP_INST_H
#define MEDEA_INPUTPARAMETERGROUP_INST_H
#include "../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class InputParameterGroupInst : public Node{
        friend class ::EntityFactory;
        public:
            static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
        protected:
            InputParameterGroupInst(EntityFactoryBroker& factory, bool is_temp_node);
            bool canAdoptChild(Node* child);
            QSet<NODE_KIND> getUserConstructableNodeKinds() const;
        private:
            bool canAdoptVariadicParameters() const;
    };
};

#endif // MEDEA_INPUTPARAMETERGROUP_INST_H
