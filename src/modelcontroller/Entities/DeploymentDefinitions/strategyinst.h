//
// Created by Cathlyn on 13/02/2020.
//

#ifndef MEDEA_STRATEGYINST_H
#define MEDEA_STRATEGYINST_H

#include "../node.h"

class EntityFactoryRegistryBroker;
class StrategyInst : public Node
{
    Q_OBJECT

public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& registry_broker);
    
protected:
    StrategyInst(EntityFactoryBroker& factory_broker, bool is_temp_node);
};


#endif //MEDEA_STRATEGYINST_H
