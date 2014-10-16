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
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);
};

#endif // ATTRIBUTE_H
