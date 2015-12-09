#ifndef BRANCHSTATE_H
#define BRANCHSTATE_H
#include "branch.h"

class BranchState: public Branch
{
    Q_OBJECT
public:
    BranchState();
    ~BranchState();

public:
    bool canAdoptChild(Node* child);

};
#endif // BRANCHSTATE_H
