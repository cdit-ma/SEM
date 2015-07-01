#ifndef OUTEVENTPORTIMPL_H
#define OUTEVENTPORTIMPL_H
#include "../node.h"
class OutEventPort;

class OutEventPortImpl : public Node
{
    Q_OBJECT
public:
    OutEventPortImpl();
    ~OutEventPortImpl();

public:
    bool canAdoptChild(Node* child);
    bool canConnect(Node* attachableObject);
    //QString toGraphML(qint32 indentationLevel=0);
    

};

#endif
