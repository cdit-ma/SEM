#ifndef DATANODE_H
#define DATANODE_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class DataNode : public Node
{

protected:
    DataNode(EntityFactoryBroker& factory, NODE_KIND kind, bool is_temp, bool use_complex_types = true);
public:
    static void BindDataRelationship(Node* source, Node* destination, bool setup);
    bool hasInputData();
    bool hasOutputData();

    DataNode* getInputData();
    DataNode* getOutputData();

    void setPromiscuousDataLinker(bool set);
    void setMultipleDataReceiver(bool receiver);
    void setMultipleDataProducer(bool receiver);

    void setDataProducer(bool producer);
    void setDataReceiver(bool receiver);
    bool isDataProducer() const;
    bool isDataReceiver() const;

    bool isPromiscuousDataLinker() const;
    bool isMultipleDataReceiver() const;
    bool isMultipleDataProducer() const;

    bool comparableTypes(DataNode* node);
    virtual bool canAcceptEdge(EDGE_KIND edge_kind, Node *dst);

    bool isContainedInVector();
    bool isContainedInVariable();
    bool isContainedInFunctionCall();
    bool isContainedInAggregateInstance();

    Node* getContainmentNode();
    Node* getChildOfContainmentNode();
private:
    void updateType();
    void RunContainmentChecks();
    bool _run_containment_checks = false;

    bool _contained_in_vector = false;
    bool _contained_in_variable = false;
    bool _contained_in_function_call = false;
    bool _contained_in_aggregate_instance = false;
    Node* _containment_node = 0;
    Node* _child_of_containment_node = 0;

    bool promiscuous_data_linker_ = false;


    bool is_producer_ = false;
    bool is_receiver_ = false;
    bool is_multiple_data_receiver_ = false;
    bool is_multiple_data_producer_ = true;

};

#endif // DATANODE_H
