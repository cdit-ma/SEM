#ifndef ASSEMBLYEDGE_H
#define ASSEMBLYEDGE_H
#include "../edge.h"


class EntityFactoryRegistryBroker;
class AssemblyEdge : public Edge
{
    Q_OBJECT


protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    AssemblyEdge(EntityFactoryBroker& factory, Node *src, Node *dst);
};
#endif // ASSEMBLYEDGE_H
