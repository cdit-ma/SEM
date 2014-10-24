#ifndef COMPONENTBEHAVIOUR_H
#define COMPONENTBEHAVIOUR_H
#include "../node.h"

#include <Qstring>

class Component;
class ComponentImpl: public Node
{
     Q_OBJECT
public:
    ComponentImpl(QString name="");
    ~ComponentImpl();    // GraphML interface
    QString toString();

    void setDefinition(Component* def);
    Component* getDefinition();

public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

private:
    Component* def;

    // Node interface

    // Node interface
public:
    void addEdge(Edge *edge);

    // Node interface
};

#endif // COMPONENTBEHAVIOUR_H


