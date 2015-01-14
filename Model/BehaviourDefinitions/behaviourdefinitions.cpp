#include "behaviourdefinitions.h"
#include "componentimpl.h"

#include <QDebug>


BehaviourDefinitions::BehaviourDefinitions():Node()
{
}

BehaviourDefinitions::~BehaviourDefinitions()
{

}

bool BehaviourDefinitions::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool BehaviourDefinitions::canAdoptChild(Node *child)
{
    ComponentImpl* component = dynamic_cast<ComponentImpl *>(child);

    if(!component){
        qWarning() << "BehaviourDefinitions can only adopt a ComponentImpl Node";
        return false;
    }

    return Node::canAdoptChild(child);
}
