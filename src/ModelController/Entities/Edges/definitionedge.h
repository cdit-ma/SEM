#ifndef DEFINITIONEDGE_H
#define DEFINITIONEDGE_H
#include "../edge.h"

class EntityFactory;
class DefinitionEdge : public Edge
{
    Q_OBJECT
    friend class EntityFactory;
protected:
    DefinitionEdge(EntityFactory* factory);
    DefinitionEdge(Node *src, Node *dst);
private:
    static DefinitionEdge* ConstructEdge(Node* src, Node* dst);
};

#endif // EDGE_H
