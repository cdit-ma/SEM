#ifndef COMPONENTASSEMBLY_H
#define COMPONENTASSEMBLY_H
#include "node.h"

class ComponentAssembly : public Node
{
public:
    ComponentAssembly(QString name);
    ~ComponentAssembly();
public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel=0);
    QString toString();
};

#endif // COMPONENTASSEMBLY_H
