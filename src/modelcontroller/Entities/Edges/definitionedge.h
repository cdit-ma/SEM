#ifndef DEFINITIONEDGE_H
#define DEFINITIONEDGE_H
#include "../edge.h"

class EntityFactory;
class DefinitionEdge : public Edge
{
    Q_OBJECT
    friend class EntityFactory;
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DefinitionEdge(EntityFactory& factory, Node *src, Node *dst);
};

#endif // EDGE_H
