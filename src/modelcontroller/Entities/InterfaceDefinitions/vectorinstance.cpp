#include "vectorinstance.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::VECTOR_INSTANCE;
const QString kind_string = "VectorInstance";

void VectorInstance::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new VectorInstance(factory, is_temp_node);
    });
}

VectorInstance::VectorInstance(EntityFactory& factory, bool is_temp) : DataNode(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setDataProducer(true);
    setDataReceiver(true);

    addInstancesDefinitionKind(NODE_KIND::VECTOR);
    setChainableDefinition();

    setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::MEMBER_INSTANCE);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "Vector", true);
    factory.AttachData(this, "outer_type", QVariant::String, "Vector", true);
    factory.AttachData(this, "icon", QVariant::String, "", true);
    factory.AttachData(this, "icon_prefix", QVariant::String, "", true);
}

bool VectorInstance::canAdoptChild(Node *child)
{
   
    if(getChildrenCount() > 0){
        return false;
    }
    return Node::canAdoptChild(child);
}