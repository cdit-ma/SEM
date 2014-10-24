#ifndef MEMBER_H
#define MEMBER_H
#include "../node.h"

#include <Qstring>

class Member : public Node
{
        Q_OBJECT
public:
    Member(QString name="");
    ~Member();

    QString toString();
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // ATTRIBUTE_H
