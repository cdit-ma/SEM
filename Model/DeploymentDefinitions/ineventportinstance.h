#ifndef INEVENTPORTINSTANCE_H
#define INEVENTPORTINSTANCE_H
#include "../node.h"

class InEventPortInstance : public Node
{
        Q_OBJECT
public:
    InEventPortInstance(QString name="");
    ~InEventPortInstance();


    // GraphML interface
public:
    bool canAdoptChild(Node* child);
    bool canConnect(Node* attachableObject);
    //QString toGraphML(qint32 indentationLevel);
    

};

#endif // INEVENTPORTINSTANCE_H
