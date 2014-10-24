#ifndef COMPONENT_H
#define COMPONENT_H
#include "../node.h"

class ComponentInstance;
class ComponentImpl;

class Component : public Node
{
public:
    Component(QString name="");
    ~Component();

    void addInstance(ComponentInstance* instance);
    void removeInstance(ComponentInstance* instance);
    QVector<ComponentInstance*> getInstances();

    void setImpl(ComponentImpl* impl);
    ComponentImpl* getImpl();

    QString toString();

    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
private:
    QVector<ComponentInstance*> instances;
    ComponentImpl* impl;


};

#endif // COMPONENT_H
