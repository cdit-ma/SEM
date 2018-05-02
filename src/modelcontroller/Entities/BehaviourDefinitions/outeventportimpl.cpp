#include "outeventportimpl.h"

#include "../../edgekinds.h"


const NODE_KIND node_kind = NODE_KIND::OUTEVENTPORT_IMPL;
const QString kind_string = "OutEventPortImpl";


OutEventPortImpl::OutEventPortImpl(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new OutEventPortImpl();});

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
};

OutEventPortImpl::OutEventPortImpl():Node(node_kind){
    addImplsDefinitionKind(NODE_KIND::OUTEVENTPORT);
    
    //Allow links from within things like InEventPortImpls back to the
    SetEdgeRuleActive(EdgeRule::MIRROR_PARENT_DEFINITION_HIERARCHY, false);

    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
}

bool OutEventPortImpl::canAdoptChild(Node *child)
{
    auto child_node_kind = child->getNodeKind();
    
    switch(child_node_kind){
        case NODE_KIND::AGGREGATE_INSTANCE:{
            if(getChildrenOfKind(child_node_kind, 0).size() > 0){
                return false;
            }
            break;
        default:
            break;
        }
    }
    return Node::canAdoptChild(child);
}
