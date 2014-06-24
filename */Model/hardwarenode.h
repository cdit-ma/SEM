#ifndef HARDWARENODE_H
#define HARDWARENODE_H
#include "node.h"

class HardwareNode : public Node
{
public:
    HardwareNode(QString name);
    ~HardwareNode();

    const static qint32 nodeKind = 1;

    // GraphML interface
public:
    bool isAdoptLegal(GraphML *child);
    bool isEdgeLegal(GraphML *attachableObject);
    QString toGraphML(qint32 indentationLevel=0);
    QString toString();
};

#endif // HARDWARENODE_H
