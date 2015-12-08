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
};

#endif // COMPONENTASSEMBLY_H
