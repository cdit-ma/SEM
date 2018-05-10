#include "whileloop.h"
#include "../containernode.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"
#include "../../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::WHILE_LOOP;
const QString kind_string = "WhileLoop";

void MEDEA::WhileLoop::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::WhileLoop(broker, is_temp_node);
        });
}

MEDEA::WhileLoop::WhileLoop(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //SetupState
    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);

    setAcceptsNodeKind(NODE_KIND::VARIABLE_PARAMETER);
    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER);

    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }
    setLabelFunctional(false);




    broker.AttachData(this, "label", QVariant::String, "while", true);
}


bool MEDEA::WhileLoop::canAdoptChild(Node *child)
{
    auto child_kind = child->getNodeKind();
    
    switch(child_kind){
        case NODE_KIND::IF_CONDITION:{
            if(getChildrenOfKindCount(child_kind) > 0){
                return false;
            }
            break;
        }
    default:
        break;
    }
    
    //Ignore the can adopt child from condition
    return Node::canAdoptChild(child);
}

