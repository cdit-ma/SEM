#ifndef MEMBER_H
#define MEMBER_H
#include "../node.h"

class Member : public Node
{
    Q_OBJECT
public:
    Member();
    ~Member();

    // GraphML interface
public:
    

    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // MEMBER_H
