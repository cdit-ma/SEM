#ifndef ASSEMBLYEDGE_H
#define ASSEMBLYEDGE_H
#include "../edge.h"

class AssemblyEdge : public Edge
{
    Q_OBJECT
protected:
    AssemblyEdge(Node *src, Node *dst);
public:
    static AssemblyEdge* createAssemblyEdge(Node* src, Node* dst);
};
#endif // ASSEMBLYEDGE_H
