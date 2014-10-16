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

    // GraphMLContainer interface
public:
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);
};

#endif // HARDWARECLUSTER_H
