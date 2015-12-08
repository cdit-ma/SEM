#ifndef COMPONENTINSTANCE_H
#define COMPONENTINSTANCE_H
#include "../node.h"
#include "../BehaviourDefinitions/componentimpl.h"


class ComponentInstance : public Node
{
        Q_OBJECT
public:
    ComponentInstance();
    ~ComponentInstance();

public:
    bool canAdoptChild(Node* child);
    Edge::EDGE_CLASS canConnect(Node* attachableObject);
    //QString toGraphML(qint32 indentationLevel=0);
    

};

#endif // COMPONENTINSTANCE_H
