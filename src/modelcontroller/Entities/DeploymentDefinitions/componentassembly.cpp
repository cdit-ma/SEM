#include "componentassembly.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::COMPONENT_ASSEMBLY;
const QString kind_string = "ComponentAssembly";

void ComponentAssembly::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new ComponentAssembly(factory, is_temp_node);
        });
}

ComponentAssembly::ComponentAssembly(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }
    
    //Setup State
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::SOURCE);
    setAcceptsEdgeKind(EDGE_KIND::QOS, EDGE_DIRECTION::SOURCE);

    setAcceptsNodeKind(NODE_KIND::COMPONENT_ASSEMBLY);
    setAcceptsNodeKind(NODE_KIND::COMPONENT_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::INEVENTPORT_DELEGATE);
    setAcceptsNodeKind(NODE_KIND::OUTEVENTPORT_DELEGATE);
    setAcceptsNodeKind(NODE_KIND::DEPLOYMENT_ATTRIBUTE);

    //Setup Data
    factory.AttachData(this, "replicate_count", QVariant::Int, 1, false);
    factory.AttachData(this, "comment", QVariant::String, "", false);
}