#ifndef COMPONENTINSTANCE_H
#define COMPONENTINSTANCE_H
#include "node.h"
#include "hardwarenode.h"


class ComponentInstance : public Node
{
public:
    ComponentInstance(QString name);
    ~ComponentInstance();
    const static qint32 nodeKind = 2;
public:
    bool isAdoptLegal(GraphML *child);
    bool isEdgeLegal(GraphML *attachableObject);
    QString toGraphML(qint32 indentationLevel=0);
    QString toString();
};

#endif // COMPONENTINSTANCE_H
