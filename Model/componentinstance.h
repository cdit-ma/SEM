#ifndef COMPONENTINSTANCE_H
#define COMPONENTINSTANCE_H
#include "node.h"
#include "hardwarenode.h"


class ComponentInstance : public Node
{
        Q_OBJECT
public:
    ComponentInstance(QString name="");
    ~ComponentInstance();
public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel=0);
    QString toString();
};

#endif // COMPONENTINSTANCE_H
