#include "branchstate.h"
#include <QDebug>

BranchState::BranchState():Node()
{
    //qCritical() << "Constructed BranchState: "<< this->getName();

}

BranchState::~BranchState()
{
    //Destructor
}

bool BranchState::canConnect(Node* attachableObject)
{
    return Node::canConnect(attachableObject);
}

bool BranchState::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}
