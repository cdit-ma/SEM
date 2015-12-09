#ifndef OUTEVENTPORTINSTANCE_H
#define OUTEVENTPORTINSTANCE_H
#include "eventportinstance.h"

class OutEventPortInstance : public EventPortInstance
{
    Q_OBJECT
public:
    OutEventPortInstance();
    ~OutEventPortInstance();

    bool canAdoptChild(Node* child);
    bool canConnect_DefinitionEdge(Node *definition);
    bool canConnect_AssemblyEdge(Node *node);
};

#endif // OUTEVENTPORTINSTANCE_H
