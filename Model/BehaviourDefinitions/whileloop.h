#ifndef WHILELOOP_H
#define WHILELOOP_H
#include "branch.h"

class WhileLoop: public Branch
{
    Q_OBJECT
public:
    WhileLoop();
    ~WhileLoop();

public:
    Edge::EDGE_CLASS canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};
#endif // WHILELOOP_H
