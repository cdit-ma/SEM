#ifndef TERMINATION_H
#define TERMINATION_H
#include "../node.h"

class Termination: public Node
{
    Q_OBJECT
public:
    Termination();
    ~Termination();

    // GraphML interface
    

    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};


#endif // TERMINATION_H
