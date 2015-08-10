#include "idl.h"
#include "component.h"
#include "aggregate.h"
#include "blackbox.h"
#include <QDebug>
IDL::IDL(): Node()
{

}

IDL::~IDL()
{

}

bool IDL::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool IDL::canAdoptChild(Node *item)
{
    Component* component = dynamic_cast<Component*>(item);
    Aggregate* aggregate = dynamic_cast<Aggregate*>(item);
    BlackBox* blackBox = dynamic_cast<BlackBox*>(item);


    if(!component && !aggregate && !blackBox){
        return false;
    }

    //Check children.
    foreach(Node* child, getChildren(0)){
        Component* cComponent = dynamic_cast<Component*>(child);
        Aggregate* cAggregate = dynamic_cast<Aggregate*>(child);
        BlackBox* cBlackBox = dynamic_cast<BlackBox*>(child);

        if(component && !cComponent){
            return false;
        }
        if(aggregate && !cAggregate){
            return false;
        }
        if(blackBox && !cBlackBox){
            return false;
        }
    }

    return Node::canAdoptChild(item);
}
