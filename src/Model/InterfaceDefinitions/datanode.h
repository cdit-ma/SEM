#ifndef DATANODE_H
#define DATANODE_H
#include "../node.h"

class EntityFactory;
class DataNode : public Node
{
protected:
    DataNode(EntityFactory* factory, NODE_KIND kind, QString kind_str);
    DataNode(NODE_KIND kind);
public:
    bool hasInputData();
    bool hasOutputData();

    DataNode* getInputData();
    DataNode* getOutputData();

    void setMultipleDataReceiver(bool receiver);
    void setDataProducer(bool producer);
    void setDataReciever(bool reciever);
    bool isDataProducer() const;
    bool isDataReciever() const;
    bool isMultipleDataReceiver() const;

    bool comparableTypes(DataNode* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst) = 0;
private:

    bool _isProducer;
    bool _isReciever;
    bool _isMultipleDataReceiver;

};

#endif // DATANODE_H
