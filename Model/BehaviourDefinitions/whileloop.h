#ifndef WHILELOOP_H
#define WHILELOOP_H
#include "branch.h"

class WhileLoop: public Branch
{
    Q_OBJECT
public:
    WhileLoop();
    ~WhileLoop();

    bool canAdoptChild(Node *node);
};
#endif // WHILELOOP_H
