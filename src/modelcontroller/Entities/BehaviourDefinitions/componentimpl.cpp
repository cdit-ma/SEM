#include "componentimpl.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::COMPONENT_IMPL;
const QString kind_string = "ComponentImpl";

void ComponentImpl::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new ComponentImpl(factory, is_temp_node);
        });
}

ComponentImpl::ComponentImpl(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }
    //Setup State
    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);
    addImplsDefinitionKind(NODE_KIND::COMPONENT);
    setNodeType(NODE_TYPE::TOP_BEHAVIOUR_CONTAINER);

    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE_IMPL);
    setAcceptsNodeKind(NODE_KIND::INEVENTPORT_IMPL);
    setAcceptsNodeKind(NODE_KIND::PERIODICEVENT);
    setAcceptsNodeKind(NODE_KIND::VARIABLE);
    setAcceptsNodeKind(NODE_KIND::HEADER);
    setAcceptsNodeKind(NODE_KIND::FUNCTION);
    setAcceptsNodeKind(NODE_KIND::CLASS_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::SERVER_PORT_IMPL);

    //Setup Data
    factory.AttachData(this, "type", QVariant::String, "", true);

}
QSet<Node*> ComponentImpl::getDependants() const{
    auto required_nodes = Node::getDependants();
    auto definition = getDefinition(true);
    if(definition){
        for(auto d : definition->getInstances()){
            required_nodes.insert(d);
        }
    }
    return required_nodes;
}
