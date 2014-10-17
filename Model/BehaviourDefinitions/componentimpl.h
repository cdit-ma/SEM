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
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);

private:
    Component* def;

    // GraphMLContainer interface

    // GraphMLContainer interface
public:
    void addEdge(Edge *edge);

    // GraphMLContainer interface
};

#endif // COMPONENTBEHAVIOUR_H


