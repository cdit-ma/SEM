#ifndef INTERFACEDEFINITIONS_H
#define INTERFACEDEFINITIONS_H

#include "../node.h"

class InterfaceDefinitions: public Node
{
    Q_OBJECT
public:
    InterfaceDefinitions();
    ~InterfaceDefinitions();

    // GraphML interface
public:


    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // INTERFACEDEFINITIONS_H
