#ifndef ASSEMBLY_H
#define ASSEMBLY_H
#include "node.h"

class Assembly : public Node
{
public:
    Assembly(QString name);
    ~Assembly();
    const static qint32 nodeKind = 3;
    // GraphML interface
public:
    bool isAdoptLegal(GraphML *child);
    bool isEdgeLegal(GraphML *attachableObject);
    QString toGraphML(qint32 indentationLevel=0);
    QString toString();
};

#endif // ASSEMBLY_H
