#ifndef INEVENTPORTDELEGATE_H
#define INEVENTPORTDELEGATE_H
#include "../node.h"
class InEventPortInstance;

class InEventPortDelegate: public Node
{
    Q_OBJECT
public:
    InEventPortDelegate();
    ~InEventPortDelegate();

    // GraphML interface
    bool connectedToInEventPortInstance();
    InEventPortInstance* getInEventPortInstance();

public:
    bool canAdoptChild(Node* child);
    bool canConnect(Node* attachableObject);
    //QString toGraphML(qint32 indentationLevel);
    

};

#endif // INEVENTPORTDELEGATE_H
