#ifndef AGGREGATE_H
#define AGGREGATE_H
#include "../node.h"

class Aggregate : public Node
{
    Q_OBJECT
public:
    Aggregate();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
private:
    QString getType();
private slots:
    void updateType(int ID, QString keyName);
};


#endif // AGGREGATE_H
