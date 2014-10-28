#ifndef AGGREGATE_H
#define AGGREGATE_H
#include "../node.h"

class AggregateMember;

class Aggregate : public Node
{
    Q_OBJECT
public:
    Aggregate();
    ~Aggregate();

    // GraphML interface
    QString toString();
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

private:
};


#endif // AGGREGATE_H
