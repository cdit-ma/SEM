#ifndef DATAEDGE_H
#define DATAEDGE_H
#include "../edge.h"

class EntityFactory;
class DataEdge : public Edge
{
    Q_OBJECT
    friend class EntityFactory;
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DataEdge(EntityFactory& factory, Node *src, Node *dst);
};

#endif // DATAEDGE_H
