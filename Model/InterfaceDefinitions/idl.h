#ifndef IDL_H
#define IDL_H
#include "../node.h"

class IDL : public Node
{
    Q_OBJECT
public:
    IDL();
    ~IDL();

    // GraphML interface
public:
    

    // Node interface
public:
    Edge::EDGE_CLASS canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

};

#endif // IDL_H
