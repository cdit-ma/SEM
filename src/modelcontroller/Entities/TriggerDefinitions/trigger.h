//
// Created by Cathlyn on 6/02/2020.
//

#ifndef MEDEA_TRIGGER_H
#define MEDEA_TRIGGER_H

#include "../node.h"

class EntityFactoryRegistryBroker;
class Trigger : public Node
{
    Q_OBJECT
    
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& registry_broker);
    VIEW_ASPECT getViewAspect() const final;

protected:
    Trigger(EntityFactoryBroker& factory_broker, bool is_temp_node);
    //void updateViewAspect(VIEW_ASPECT)
};

#endif //MEDEA_TRIGGER_H
