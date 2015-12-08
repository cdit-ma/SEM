#ifndef IDL_H
#define IDL_H
#include "../node.h"

class IDL : public Node
{
    Q_OBJECT
public:
    IDL();
    ~IDL();

    bool canAdoptChild(Node* child);
};

#endif // IDL_H
