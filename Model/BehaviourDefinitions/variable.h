#ifndef VARIABLE_H
#define VARIABLE_H

#include "../node.h"

class Variable: public Node
{
    Q_OBJECT
public:
    Variable();
    ~Variable();
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // VARIABLE_H
