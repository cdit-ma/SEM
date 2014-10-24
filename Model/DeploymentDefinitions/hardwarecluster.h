#ifndef HARDWARECLUSTER_H
#define HARDWARECLUSTER_H
#include "../node.h"

class HardwareCluster : public Node
{
public:
    HardwareCluster(QString name="");
    ~HardwareCluster();

    // GraphML interface
public:
    QString toString();

    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // HARDWARECLUSTER_H
