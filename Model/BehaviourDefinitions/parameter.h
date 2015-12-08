#ifndef PARAMETER_H
#define PARAMETER_H
#include "behaviournode.h"
class BehaviourNode;

class Parameter: public BehaviourNode
{
    Q_OBJECT
public:
    Parameter(bool isInput=false);

    bool isInputParameter();
    bool isReturnParameter();
    bool hasConnection();

    bool compareableTypes(Node* node);

    // Node interface
public:
    bool canAdoptChild(Node *node);
    bool canConnect_DataEdge(Node *node);
private:
    bool inputParameter;
};

#endif // PARAMETER_H
