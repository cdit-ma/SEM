#ifndef INEVENTPORTINSTANCE_H
#define INEVENTPORTINSTANCE_H
#include "../node.h"

class InEventPortInstance : public Node
{
        Q_OBJECT
public:
    InEventPortInstance();
    ~InEventPortInstance();


    // GraphML interface
public:
    bool canAdoptChild(Node* child);
    Edge::EDGE_CLASS canConnect(Node* attachableObject);
    //QString toGraphML(qint32 indentationLevel);
    

};

#endif // INEVENTPORTINSTANCE_H
