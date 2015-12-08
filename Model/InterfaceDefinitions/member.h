#ifndef MEMBER_H
#define MEMBER_H
#include "../node.h"

class Member : public Node
{
    Q_OBJECT
public:
    Member();
    ~Member();

    bool canAdoptChild(Node* child);
};

#endif // MEMBER_H
