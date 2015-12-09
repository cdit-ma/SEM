#ifndef INEVENTPORTINSTANCE_H
#define INEVENTPORTINSTANCE_H
#include "eventportinstance.h"

class InEventPortInstance : public EventPortInstance
{
    Q_OBJECT
public:
    InEventPortInstance();
    ~InEventPortInstance();

    bool canAdoptChild(Node*);
    bool canConnect_DefinitionEdge(Node *definition);
    bool canConnect_AssemblyEdge(Node *node);
};

#endif // INEVENTPORTINSTANCE_H
