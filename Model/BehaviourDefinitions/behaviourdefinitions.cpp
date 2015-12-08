#include "behaviourdefinitions.h"
#include "componentimpl.h"

#include <QDebug>


BehaviourDefinitions::BehaviourDefinitions():Node(Node::NT_ASPECT)
{
}

BehaviourDefinitions::~BehaviourDefinitions()
{

}

Edge::EDGE_CLASS BehaviourDefinitions::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool BehaviourDefinitions::canAdoptChild(Node *child)
{
    ComponentImpl* component = dynamic_cast<ComponentImpl *>(child);

    if(!component){
        #ifdef DEBUG_MODE
        qWarning() << "BehaviourDefinitions can only adopt a ComponentImpl Node";
#endif
        return false;
    }

    return Node::canAdoptChild(child);
}
