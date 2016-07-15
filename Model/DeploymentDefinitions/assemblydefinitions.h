#ifndef ASSEMBLYDEFINITIONS_H
#define ASSEMBLYDEFINITIONS_H
#include "../node.h"

class AssemblyDefinitions: public Node
{
    Q_OBJECT
public:
    AssemblyDefinitions();
    ~AssemblyDefinitions();

    VIEW_ASPECT getViewAspect();
    bool canAdoptChild(Node* node);
};
#endif // AssemblyDefinitions_H
