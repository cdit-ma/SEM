#include "idl.h"
#include "component.h"
#include "aggregate.h"
#include "blackbox.h"
#include "vector.h"
#include <QDebug>
IDL::IDL(): Node()
{

}

IDL::~IDL()
{

}

Edge::EDGE_CLASS IDL::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool IDL::canAdoptChild(Node *item)
{
    Component* component = dynamic_cast<Component*>(item);
    Aggregate* aggregate = dynamic_cast<Aggregate*>(item);
    BlackBox* blackBox = dynamic_cast<BlackBox*>(item);
    Vector* vector = dynamic_cast<Vector*>(item);


    if(!component && !aggregate && !blackBox && !vector){
        return false;
    }





    //Check children.
    foreach(Node* child, getChildren(0)){
        Component* cComponent = dynamic_cast<Component*>(child);
        Aggregate* cAggregate = dynamic_cast<Aggregate*>(child);
        BlackBox* cBlackBox = dynamic_cast<BlackBox*>(child);
        Vector* cVector = dynamic_cast<Vector*>(child);

        //Allow Vectors
        if(vector){
            if(!cAggregate && !cVector){
                return false;
            }else{
                continue;
            }
        }

        if(component && !cComponent){
            return false;
        }
        if(aggregate && (!cAggregate && !cVector)){
            return false;
        }
        if(blackBox && !cBlackBox){
            return false;
        }
    }

    return Node::canAdoptChild(item);
}
