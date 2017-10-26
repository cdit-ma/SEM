#ifndef ASSEMBLYEDGE_H
#define ASSEMBLYEDGE_H
#include "../edge.h"

class EntityFactory;
class AssemblyEdge : public Edge
{
    Q_OBJECT
    friend class EntityFactory;
protected:
    AssemblyEdge(EntityFactory* factory);
    AssemblyEdge(Node *src, Node *dst);
private:
    static AssemblyEdge* ConstructEdge(Node* src, Node* dst);
};
#endif // ASSEMBLYEDGE_H
