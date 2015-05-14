#ifndef COMPONENTBEHAVIOUR_H
#define COMPONENTBEHAVIOUR_H
#include "../node.h"

#include <QString>

class Component;
class ComponentImpl: public Node
{
     Q_OBJECT
public:
    ComponentImpl(QString name="");
    ~ComponentImpl();    // GraphML interface
    

public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

};

#endif // COMPONENTBEHAVIOUR_H


