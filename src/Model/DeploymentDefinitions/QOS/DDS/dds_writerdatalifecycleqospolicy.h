#ifndef DDS_WRITERDATALIFECYCLEQOSPOLICY_H
#define DDS_WRITERDATALIFECYCLEQOSPOLICY_H
#include "../../../node.h"

class DDS_WriterDataLifecycleQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_WriterDataLifecycleQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
    QList<Data*> getDefaultData();
};
#endif // DDS_WRITERDATALIFECYCLEQOSPOLICY_H


