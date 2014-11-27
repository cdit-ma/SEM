#include "behaviourdefinitions.h"
#include "componentimpl.h"

#include <QDebug>


BehaviourDefinitions::BehaviourDefinitions(QString name):Node()
{

}

BehaviourDefinitions::~BehaviourDefinitions()
{

}

QString BehaviourDefinitions::toString()
{
    return QString("BehaviourDefinitions[%1]: "+this->getName()).arg(this->getID());
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
