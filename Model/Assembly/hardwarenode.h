#ifndef HARDWARENODE_H
#define HARDWARENODE_H
#include "../node.h"

class HardwareNode : public Node
{
    Q_OBJECT
public:
    HardwareNode(QString name="");
    ~HardwareNode();
public:
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);
    QString toString();
};

#endif // HARDWARENODE_H
