//
// Created by Cathlyn Aston on 3/2/21.
//

#include "fpgafftdevice.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::FPGA_FFT_DEVICE;
const QString kind_string = "FPGA FFT Device";

/**
 * FPGAFFTDevice::RegisterWithEntityFactory
 * @param broker
 */
void FPGAFFTDevice::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker)
{
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node) {
        return new FPGAFFTDevice(broker, is_temp_node);
    });
}

/**
 * FPGAFFTDevice::FPGAFFTDevice
 * @param broker
 * @param is_temp
 */
FPGAFFTDevice::FPGAFFTDevice(EntityFactoryBroker& broker, bool is_temp)
    : Node(broker, node_kind, is_temp)
{
    // Setup State
    setLabelFunctional(false);
    setNodeType(NODE_TYPE::HARDWARE);

    // Break out early for temporary entities
    if (is_temp) {
        return;
    }

    // Setup Data
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::IpAddress, QVariant::String, ProtectedState::PROTECTED);
}