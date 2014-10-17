#include "file.h"
#include "component.h"
#include "aggregate.h"
#include <QDebug>
File::File(QString name): Node(name)
{

}

File::~File()
{

}

QString File::toString()
{
    return QString("File[%1]: "+this->getName()).arg(this->getID());
}

bool File::isEdgeLegal(GraphMLContainer *attachableObject)
{
    return false;
}

bool File::isAdoptLegal(GraphMLContainer *item)
{
    Component* component = dynamic_cast<Component*>(item);
    Aggregate* aggregate = dynamic_cast<Aggregate*>(item);

    if(!component && !aggregate){
        return false;
    }

    //Check children.
    foreach(GraphMLContainer* child, getChildren(0)){
        Component* cComponent = dynamic_cast<Component*>(child);
        Aggregate* cAggregate = dynamic_cast<Aggregate*>(child);

        if(!((component && cComponent) || (aggregate && cAggregate))){
            return false;
        }
    }

    return true;
}
