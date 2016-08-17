#include "component.h"
#include "attribute.h"
#include "eventport.h"

Component::Component(): Node(Node::NT_DEFINITION)
{
    setAcceptsEdgeClass(Edge::EC_DEFINITION);
}

Component::~Component()
{
}

bool Component::canAdoptChild(Node *child)
{
    Attribute* attribute  = dynamic_cast<Attribute*>(child);
    EventPort* eventPort  = dynamic_cast<EventPort*>(child);

    if(!(attribute || eventPort)){
        return false;
    }

    return Node::canAdoptChild(child);
}
