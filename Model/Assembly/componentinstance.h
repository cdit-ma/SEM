#ifndef COMPONENTINSTANCE_H
#define COMPONENTINSTANCE_H
#include "../node.h"


class Component;
class ComponentInstance : public Node
{
        Q_OBJECT
public:
    ComponentInstance(QString name="");
    ~ComponentInstance();
    void setComponentParent(Component *parent);
    Component* getComponentParent();
public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel=0);
    QString toString();

private:
    Component* parentComponent;
};

#endif // COMPONENTINSTANCE_H
