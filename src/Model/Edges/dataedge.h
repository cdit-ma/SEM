#ifndef DATAEDGE_H
#define DATAEDGE_H
#include "../edge.h"

class EntityFactory;
class DataEdge : public Edge
{
    Q_OBJECT
    friend class EntityFactory;
protected:
    DataEdge(EntityFactory* factory);
    DataEdge(Node *src, Node *dst);
private:
    static DataEdge* ConstructEdge(Node* src, Node* dst);
};

#endif // DATAEDGE_H
