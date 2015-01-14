#ifndef COMPONENTASSEMBLY_H
#define COMPONENTASSEMBLY_H
#include "../node.h"

class ComponentAssembly : public Node
{
    Q_OBJECT
public:
    ComponentAssembly();
    ~ComponentAssembly();
public:
    bool canAdoptChild(Node* child);
    bool canConnect(Node* attachableObject);
    //QString toGraphML(qint32 indentationLevel=0);

};

#endif // COMPONENTASSEMBLY_H
