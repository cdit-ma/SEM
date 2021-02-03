//
// Created by Cathlyn Aston on 3/2/21.
//

#ifndef FPGAFFTDEVICE_H
#define FPGAFFTDEVICE_H

#include "../node.h"

class EntityFactoryRegistryBroker;

class FPGAFFTDevice : public Node
{
    Q_OBJECT

public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);

protected:
    FPGAFFTDevice(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // FPGAFFTDEVICE_H
