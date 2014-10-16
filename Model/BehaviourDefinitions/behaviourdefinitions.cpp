#include "behaviourdefinitions.h"
#include "component.h"
#include <QDebug>


BehaviourDefinitions::BehaviourDefinitions(QString name):Node(name)
{

}

BehaviourDefinitions::~BehaviourDefinitions()
{

}

QString BehaviourDefinitions::toString()
{
    return QString("BehaviourDefinitions[%1]: "+this->getName()).arg(this->getID());
}

bool BehaviourDefinitions::isEdgeLegal(GraphMLContainer *attachableObject)
{
    return false;
}

bool BehaviourDefinitions::isAdoptLegal(GraphMLContainer *child)
{
    Component* component = dynamic_cast<Component *>(child);

    if(component == 0){
        qWarning() << "Behaviour Definitions can only adopt a Component Node";
        return true;

    }

    return true;
}
