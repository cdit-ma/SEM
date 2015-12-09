#ifndef BLANKNODE_H
#define BLANKNODE_H
#include "node.h"
class BlankNode: public Node
{
    Q_OBJECT
public:
    BlankNode();
    ~BlankNode();

    bool canAdoptChild(Node* child);
};

#endif // BLANKNODE_H
