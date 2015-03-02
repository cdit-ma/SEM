#include "termination.h"
#include <QDebug>
Termination::Termination():Node()
{
    //qWarning() << "Constructed Termination: "<< this->getName();
}

Termination::~Termination()
{
    //Destructor
}

bool Termination::canConnect(Node* attachableObject)
{
    Node* parentNode = getParentNode();
    if(parentNode){
        if(!parentNode->isAncestorOf(attachableObject)){
            return false;
        }
    }

    return Node::canConnect(attachableObject);
}

bool Termination::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}
