#include "enummember.h"
#include "enuminstance.h"
#include <QDebug>
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::ENUM_MEMBER;
const QString kind_string = "EnumMember";

void EnumMember::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new EnumMember(broker, is_temp_node);
    });
}

EnumMember::EnumMember(EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setPromiscuousDataLinker(true);
    setDataProducer(true);



    //Setup Data
    broker.AttachData(this, "index", QVariant::Int, -1, true);
    broker.AttachData(this, "type", QVariant::String, "", true);
}