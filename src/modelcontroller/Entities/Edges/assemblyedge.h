#ifndef ASSEMBLYEDGE_H
#define ASSEMBLYEDGE_H
#include "../edge.h"

class EntityFactory;
class AssemblyEdge : public Edge
{
    Q_OBJECT
    friend class EntityFactory;
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    AssemblyEdge(EntityFactory& factory, Node *src, Node *dst);
};
#endif // ASSEMBLYEDGE_H
