#ifndef COMPONENTINSTANCE_H
#define COMPONENTINSTANCE_H
#include "../node.h"
#include "../BehaviourDefinitions/componentimpl.h"


class ComponentInstance : public Node
{
        Q_OBJECT
public:
    ComponentInstance(QString name="");
    ~ComponentInstance();

public:
    bool canAdoptChild(Node* child);
    bool canConnect(Node* attachableObject);
    //QString toGraphML(qint32 indentationLevel=0);
    

};

#endif // COMPONENTINSTANCE_H
