#include "blanknode.h"
BlankNode::BlankNode():Node()
{
}

BlankNode::~BlankNode()
{
}


bool BlankNode::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}
