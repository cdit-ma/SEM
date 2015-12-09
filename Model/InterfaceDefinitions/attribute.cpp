#include "attribute.h"

Attribute::Attribute():Node(Node::NT_DEFINITION)
{
}

Attribute::~Attribute()
{
}

bool Attribute::canAdoptChild(Node*)
{
    return false;
}
