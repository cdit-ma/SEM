#ifndef VECTOR_H
#define VECTOR_H
#include "datanode.h"

class EntityFactory;
class Vector : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Vector(EntityFactory* factory);
	Vector();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
private:
    QString getVectorType();
private slots:
    void updateType();
    void childrenChanged();

};


#endif // AGGREGATE_H

