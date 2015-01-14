#include "file.h"
#include "component.h"
#include "aggregate.h"
#include <QDebug>
File::File(QString name): Node()
{

}

File::~File()
{

}

bool File::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool File::canAdoptChild(Node *item)
{
    Component* component = dynamic_cast<Component*>(item);
    Aggregate* aggregate = dynamic_cast<Aggregate*>(item);

    if(!component && !aggregate){
        return false;
    }


    //Check children.
    foreach(Node* child, getChildren(0)){
        Component* cComponent = dynamic_cast<Component*>(child);
        Aggregate* cAggregate = dynamic_cast<Aggregate*>(child);

        if(!((component && cComponent) || (aggregate && cAggregate))){
            return false;
        }
    }

    return Node::canAdoptChild(item);
}
