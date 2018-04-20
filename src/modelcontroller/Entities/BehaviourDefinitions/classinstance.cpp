#include "classinstance.h"

const NODE_KIND node_kind = NODE_KIND::CLASS_INSTANCE;
const QString kind_string = "ClassInstance";

MEDEA::ClassInstance::ClassInstance(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ClassInstance();});

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "icon_prefix", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "icon", QVariant::String, true);
};

MEDEA::ClassInstance::ClassInstance(): Node(node_kind)
{
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setNodeType(NODE_TYPE::INSTANCE);
    setNodeType(NODE_TYPE::DEFINITION);

    setDefinitionKind(NODE_KIND::CLASS);
    setInstanceKind(NODE_KIND::CLASS_INSTANCE);

    SetEdgeRuleActive(Node::EdgeRule::MIRROR_PARENT_DEFINITION_HIERARCHY, false);
}

bool MEDEA::ClassInstance::ClassInstance::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
        case NODE_KIND::ATTRIBUTE:
            break;
        case NODE_KIND::ATTRIBUTE_INSTANCE:
            if(getViewAspect() == VIEW_ASPECT::BEHAVIOUR){
                return false;
            }
            break;
        case NODE_KIND::FUNCTION:
            if(getViewAspect() != VIEW_ASPECT::BEHAVIOUR){
                return false;
            }
            break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool MEDEA::ClassInstance::ClassInstance::canAcceptEdge(EDGE_KIND edge_kind, Node* dst)
{
    if(!requiresEdgeKind(edge_kind)){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        switch(dst->getNodeKind()){
            case NODE_KIND::CLASS:
            case NODE_KIND::CLASS_INSTANCE:{
                if(getViewAspect() == VIEW_ASPECT::BEHAVIOUR){
                    if(dst->getViewAspect() != VIEW_ASPECT::BEHAVIOUR) {
                        return false;
                    }
                }
                break;
            }
            default:
                return false;
        }
        break;
    }
    default:
        break;
    }

    return Node::canAcceptEdge(edge_kind, dst);
}
