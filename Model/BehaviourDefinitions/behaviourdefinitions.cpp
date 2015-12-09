#include "behaviourdefinitions.h"
#include "componentimpl.h"

BehaviourDefinitions::BehaviourDefinitions():Node(Node::NT_ASPECT)
{
}

BehaviourDefinitions::~BehaviourDefinitions()
{
}

bool BehaviourDefinitions::canAdoptChild(Node *child)
{
    ComponentImpl* component = dynamic_cast<ComponentImpl *>(child);

    if(!component){
        return false;
    }

    return Node::canAdoptChild(child);
}
