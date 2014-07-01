#ifndef ASSEMBLY_H
#define ASSEMBLY_H
#include "node.h"

class Assembly : public Node
{
public:
    Assembly(QString name);
    ~Assembly();
public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    QString toGraphML(qint32 indentationLevel=0);
    QString toString();
};

#endif // ASSEMBLY_H
