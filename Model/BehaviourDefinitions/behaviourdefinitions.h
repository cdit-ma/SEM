#ifndef BEHAVIOURDEFINITIONS_H
#define BEHAVIOURDEFINITIONS_H

#include "../node.h"

class BehaviourDefinitions: public Node
{
    Q_OBJECT
public:
    BehaviourDefinitions(QString name ="");
    ~BehaviourDefinitions();


    // GraphML interface
public:
    QString toString();

    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // BEHAVIOURDEFINITIONS_H
