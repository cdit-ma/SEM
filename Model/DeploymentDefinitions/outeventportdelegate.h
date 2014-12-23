#ifndef OUTEVENTPORTDELEGATE_H
#define OUTEVENTPORTDELEGATE_H
#include "../node.h"

class OutEventPortInstance;

class OutEventPortDelegate: public Node
{
    Q_OBJECT
public:
    OutEventPortDelegate();
    ~OutEventPortDelegate();

    bool connectedToOutEventPortInstance();
    OutEventPortInstance* getOutEventPortInstance();

    // GraphML interface
public:
    bool canAdoptChild(Node* child);
    bool canConnect(Node* attachableObject);
    //QString toGraphML(qint32 indentationLevel);
    QString toString();

};


#endif // OUTEVENTPORTDELEGATE_H
