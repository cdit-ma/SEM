#ifndef DATANODE_H
#define DATANODE_H
#include "../node.h"

class DataNode : public Node
{
public:
    DataNode(NODE_KIND kind);

    bool hasInputData();
    bool hasOutputData();

    DataNode* getInputData();
    DataNode* getOutputData();

    void setDataProducer(bool producer);
    void setDataReciever(bool reciever);
    bool isDataProducer() const;
    bool isDataReciever() const;

    bool comparableTypes(DataNode* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst) = 0;
private:

    bool _isProducer;
    bool _isReciever;

};

#endif // DATANODE_H
