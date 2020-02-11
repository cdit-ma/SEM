//
// Created by Cathlyn on 11/02/2020.
//

#ifndef MEDEA_TRIGGERDEFINITIONS_H
#define MEDEA_TRIGGERDEFINITIONS_H

#include "../node.h"

class EntityFactoryRegistryBroker;
class TriggerDefinitions : public Node
{
    Q_OBJECT
    
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& registry_broker);
    
    VIEW_ASPECT getViewAspect() const final;
    
protected:
    TriggerDefinitions(EntityFactoryBroker& factory_broker, bool is_temp_node);
    
    void updateViewAspect(VIEW_ASPECT) final {} ;
    
};


#endif //MEDEA_TRIGGERDEFINITIONS_H
