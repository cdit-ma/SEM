#ifndef INEVENTPORTDELEGATE_H
#define INEVENTPORTDELEGATE_H
#include "../node.h"
class InEventPortDelegate
{
    Q_OBJECT
public:
    InEventPortDelegate();
    ~InEventPortDelegate();

    // GraphML interface
public:
    bool canAdoptChild(Node* child);
    bool canConnect(Node* attachableObject);
    //QString toGraphML(qint32 indentationLevel);
    QString toString();

};

#endif // INEVENTPORTDELEGATE_H
