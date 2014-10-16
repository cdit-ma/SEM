#ifndef COMPONENT_H
#define COMPONENT_H
#include "../node.h"
#include "../DeploymentDefinitions/componentinstance.h"

#include <Qstring>

class ComponentInstance;
class Component : public Node
{
     Q_OBJECT
public:
    Component(QString name="");
    ~Component();

    // GraphML interface
    QString toString();

    void addComponentInstance(ComponentInstance* child);
    void removeComponentInstance(ComponentInstance* child);
    QVector<ComponentInstance*> getComponentInstances();
    // GraphMLContainer interface
public:
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);

private:
    QVector<ComponentInstance*> componentInstances;


    // GraphMLContainer interface

    // GraphMLContainer interface
public:
    void addEdge(Edge *edge);

    // GraphMLContainer interface
};

#endif // COMPONENT_H


