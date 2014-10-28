#ifndef AGGREGATEMEMBER_H
#define AGGREGATEMEMBER_H
#include "../node.h"

class Aggregate;

class AggregateMember : public Node
{
    Q_OBJECT
public:
    AggregateMember(QString name ="");
    ~AggregateMember();
    // GraphML interface
public:
    QString toString();

    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

private:
    Aggregate* def;
};



#endif // AGGREGATEMEMBER_H
