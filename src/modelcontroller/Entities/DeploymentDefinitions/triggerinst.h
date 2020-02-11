//
// Created by Cathlyn on 6/02/2020.
//

#ifndef MEDEA_TRIGGERINST_H
#define MEDEA_TRIGGERINST_H

#include "../node.h"

class EntityFactoryRegistryBroker;
class TriggerInst : public Node
{
    Q_OBJECT

public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& registry_broker);

protected:
    TriggerInst(EntityFactoryBroker& factory_broker, bool is_temp_node);

    //QSet<Node*> getListOfValidAncestorsForChildrenDefinitions() final;
};


#endif //MEDEA_TRIGGERINST_H
