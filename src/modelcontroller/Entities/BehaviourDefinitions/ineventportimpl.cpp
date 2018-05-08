#include "ineventportimpl.h"
#include "../../entityfactory.h"
#include "../../edgekinds.h"

const NODE_KIND node_kind = NODE_KIND::INEVENTPORT_IMPL;
const QString kind_string = "InEventPortImpl";


void InEventPortImpl::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new InEventPortImpl(factory, is_temp_node);
        });
}

InEventPortImpl::InEventPortImpl(EntityFactory& factory, bool is_temp) : Node(node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    addImplsDefinitionKind(NODE_KIND::INEVENTPORT);

    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }

    //Setup Data
    factory.AttachData(this, "type", QVariant::String, "", true);
}

bool InEventPortImpl::canAdoptChild(Node *child)
{
    auto child_kind = child->getNodeKind();
    
    switch(child_kind){
        case NODE_KIND::AGGREGATE_INSTANCE:{
            if(getChildrenOfKindCount(child_kind) > 0){
                return false;
            }
            break;
        default:
            break;
        }
    }
    return Node::canAdoptChild(child);
}
