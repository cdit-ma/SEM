#include "interfacedefinitions.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::INTERFACE_DEFINITIONS;
const QString kind_string = "InterfaceDefinitions";

void InterfaceDefinitions::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new InterfaceDefinitions(factory, is_temp_node);
        });
}

InterfaceDefinitions::InterfaceDefinitions(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }
    
    //Setup State
    setNodeType(NODE_TYPE::ASPECT);
    setAcceptsNodeKind(NODE_KIND::ENUM);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE);
    setAcceptsNodeKind(NODE_KIND::COMPONENT);
    setAcceptsNodeKind(NODE_KIND::SHARED_DATATYPES);
    setAcceptsNodeKind(NODE_KIND::NAMESPACE);
    setAcceptsNodeKind(NODE_KIND::SERVER_INTERFACE);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "INTERFACES", true);
}

VIEW_ASPECT InterfaceDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::INTERFACES;
}