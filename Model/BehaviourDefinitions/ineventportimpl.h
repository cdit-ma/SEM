#ifndef INEVENTPORTIMPL_H
#define INEVENTPORTIMPL_H
#include "../node.h"

class InEventPortImpl : public Node
{
    Q_OBJECT
public:
    InEventPortImpl(QString name="");
    ~InEventPortImpl();

public:
    bool canAdoptChild(Node* child);
    bool canConnect(Node* attachableObject);
    //QString toGraphML(qint32 indentationLevel=0);
    QString toString();

};


#endif // INEVENTPORT_H
