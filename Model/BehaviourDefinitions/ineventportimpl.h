#ifndef INEVENTPORTIMPL_H
#define INEVENTPORTIMPL_H
#include "../node.h"

class InEventPortImpl : public Node
{
    Q_OBJECT
public:
    InEventPortImpl();
    ~InEventPortImpl();

public:
    bool canAdoptChild(Node* child);
    bool canConnect(Node* attachableObject);
    //QString toGraphML(qint32 indentationLevel=0);


};


#endif // INEVENTPORT_H
