#include "periodicevent.h"
#include <QDebug>
PeriodicEvent::PeriodicEvent():Node()
{

}

PeriodicEvent::~PeriodicEvent()
{
    //Destructor
}

bool PeriodicEvent::canConnect(Node* attachableObject)
{
    Node* parentNode = getParentNode();
    if(parentNode){
        if(!parentNode->isAncestorOf(attachableObject)){
            return false;
        }
    }
    //Only in things locally contained.
    return Node::canConnect(attachableObject);
}

bool PeriodicEvent::canAdoptChild(Node *child)
{
    return false;
}
