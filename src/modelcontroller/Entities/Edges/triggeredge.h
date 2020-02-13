//
// Created by Cathlyn on 13/02/2020.
//

#ifndef MEDEA_TRIGGEREDGE_H
#define MEDEA_TRIGGEREDGE_H

#include "../edge.h"

class EntityFactoryRegistryBroker;
class TriggerEdge : public Edge
{
    Q_OBJECT

public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& registry_broker);

protected:
    TriggerEdge(EntityFactoryBroker& factory_broker, Node *src, Node *dst);
};


#endif //MEDEA_TRIGGEREDGE_H
