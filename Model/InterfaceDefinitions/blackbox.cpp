#include "blackbox.h"
#include "eventport.h"

BlackBox::BlackBox():Node(Node::NT_DEFINITION)
{
}

BlackBox::~BlackBox()
{
}


bool BlackBox::canAdoptChild(Node *child)
{
    EventPort* eventPort  = dynamic_cast<EventPort*>(child);

    if(!eventPort){
        return false;
    }

    return Node::canAdoptChild(child);
}
