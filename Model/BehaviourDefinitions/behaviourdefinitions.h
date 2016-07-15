#ifndef BEHAVIOURDEFINITIONS_H
#define BEHAVIOURDEFINITIONS_H

#include "../node.h"

class BehaviourDefinitions: public Node
{
    Q_OBJECT
public:
    BehaviourDefinitions();
    ~BehaviourDefinitions();
    VIEW_ASPECT getViewAspect();

public:
    bool canAdoptChild(Node* child);
};

#endif // BEHAVIOURDEFINITIONS_H
