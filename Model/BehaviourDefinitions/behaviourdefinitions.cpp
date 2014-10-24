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
    return false;
}

bool BehaviourDefinitions::canAdoptChild(Node *child)
{
    ComponentImpl* component = dynamic_cast<ComponentImpl *>(child);

    if(component == 0){
        qWarning() << "Behaviour Definitions can only adopt a Component Node";
        return true;

    }

    return true;
}
