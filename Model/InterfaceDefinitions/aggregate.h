#ifndef AGGREGATE_H
#define AGGREGATE_H
#include "../node.h"

class AggregateMember;

class Aggregate : public Node
{
    Q_OBJECT
public:
    Aggregate(QString name ="");
    ~Aggregate();

    void addMember(AggregateMember* member);
    void removeMember(AggregateMember* member);
    QVector<AggregateMember*> getMembers();

    // GraphML interface
public:
    QString toString();

    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

private:
    QVector<AggregateMember*> members;
};


#endif // AGGREGATE_H
