#ifndef BRANCHSTATE_H
#define BRANCHSTATE_H
#include "../node.h"

class BranchState: public Node
{
    Q_OBJECT
public:
    BranchState();
    ~BranchState();

    // GraphML interface
    

    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};
#endif // BRANCHSTATE_H
