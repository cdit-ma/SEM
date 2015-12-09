#include "member.h"

Member::Member(): Node(Node::NT_DEFINITION)
{
}

Member::~Member()
{
}

bool Member::canAdoptChild(Node*)
{
    return false;
}
