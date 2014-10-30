#ifndef CONDITION_H
#define CONDITION_H
#include "../node.h"

class Condition: public Node
{
    Q_OBJECT
public:
    Condition();
    ~Condition();

    // GraphML interface
    QString toString();

    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // CONDITION_H
