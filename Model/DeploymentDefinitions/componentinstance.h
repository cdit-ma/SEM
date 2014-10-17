#ifndef COMPONENTINSTANCE_H
#define COMPONENTINSTANCE_H
#include "../node.h"
#include "../BehaviourDefinitions/componentimpl.h"

class Component;
class ComponentImpl;

class ComponentInstance : public Node
{
        Q_OBJECT
public:
    ComponentInstance(QString name="");
    ~ComponentInstance();

    void setDefinition(Component* def);
    Component* getDefinition();

    ComponentImpl* getImpl();

public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel=0);
    QString toString();

private:
    Component* def;
};

#endif // COMPONENTINSTANCE_H
