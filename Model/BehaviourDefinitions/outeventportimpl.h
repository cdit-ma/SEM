#ifndef OUTEVENTPORTIMPL_H
#define OUTEVENTPORTIMPL_H
#include "eventportimpl.h"

class OutEventPortImpl : public EventPortImpl
{
    Q_OBJECT
public:
    OutEventPortImpl();
    ~OutEventPortImpl();

public:
    bool canConnect_DefinitionEdge(Node *definition);
};

#endif // OUTEVENTPORTIMPL_H
