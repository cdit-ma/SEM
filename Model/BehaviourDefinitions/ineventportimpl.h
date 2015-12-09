#ifndef INEVENTPORTIMPL_H
#define INEVENTPORTIMPL_H
#include "eventportimpl.h"

class InEventPortImpl : public EventPortImpl
{
    Q_OBJECT
public:
    InEventPortImpl();
    ~InEventPortImpl();

public:
    bool canConnect_DefinitionEdge(Node *definition);
};


#endif // INEVENTPORT_H
