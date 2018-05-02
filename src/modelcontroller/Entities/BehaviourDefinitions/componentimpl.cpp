#include "componentimpl.h"

#include "../../edgekinds.h"



ComponentImpl::ComponentImpl(EntityFactory* factory) : Node(factory, NODE_KIND::COMPONENT_IMPL, "ComponentImpl"){
	auto node_kind = NODE_KIND::COMPONENT_IMPL;
	QString kind_string = "ComponentImpl";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ComponentImpl();});

    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
};

ComponentImpl::ComponentImpl():Node(NODE_KIND::COMPONENT_IMPL){
    addImplsDefinitionKind(NODE_KIND::COMPONENT);

    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE_IMPL);
    setAcceptsNodeKind(NODE_KIND::INEVENTPORT_IMPL);
    setAcceptsNodeKind(NODE_KIND::PERIODICEVENT);
    setAcceptsNodeKind(NODE_KIND::VARIABLE);
    setAcceptsNodeKind(NODE_KIND::HEADER);
    setAcceptsNodeKind(NODE_KIND::FUNCTION);
    setAcceptsNodeKind(NODE_KIND::CLASS_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::SERVER_PORT_IMPL);
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
