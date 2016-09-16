#include "datanode.h"
#include "vectorinstance.h"
#include <QDebug>

DataNode::DataNode(Node::NODE_KIND kind):Node(kind)
{
    setAcceptsEdgeKind(Edge::EC_DATA);
    setNodeType(NT_DATA);
    _isProducer = false;
    _isReciever = false;
}

bool DataNode::hasInputData()
{
   return getInputData();
}

bool DataNode::hasOutputData()
{
    return getOutputData();
}

DataNode *DataNode::getInputData()
{
    foreach(Edge* edge, getEdges(0, Edge::EC_DATA)){
        if(edge->getDestination() == this){
            return (DataNode*) edge->getSource();
        }
    }
    return 0;
}

DataNode *DataNode::getOutputData()
{
    foreach(Edge* edge, getEdges(0, Edge::EC_DATA)){
        if(edge->getSource() == this){
            return (DataNode*) edge->getDestination();
        }
    }
    return 0;
}

void DataNode::setDataProducer(bool producer)
{
    _isProducer = producer;
}

void DataNode::setDataReciever(bool reciever)
{
    _isReciever = reciever;
}

bool DataNode::isDataProducer() const
{
    return _isProducer;
}

bool DataNode::isDataReciever() const
{
    return _isReciever;
}

bool DataNode::comparableTypes(DataNode *node)
{
    QStringList numberTypes;
    numberTypes << "ShortInteger" << "LongInteger" << "LongLongInteger";
    numberTypes << "UnsignedShortInteger" << "UnsignedLongInteger" << "UnsignedLongLongInteger";
    numberTypes << "FloatNumber" << "DoubleNumber" << "LongDoubleNumber";
    numberTypes << "Boolean" << "Byte";

    QStringList stringTypes;
    stringTypes << "String" << "WideString";

    if(node){

        //Types
        QString type1 = getDataValue("type").toString();
        QString type2 = node->getDataValue("type").toString();

        if(type1 == type2 && type1 != ""){
            //Allow direct matches.
            return true;
        }
        if(numberTypes.contains(type1) && numberTypes.contains(type2)){
            //Allow matches of numbers
            return true;
        }
        if(stringTypes.contains(type1) && stringTypes.contains(type2)){
            //Allow matches of Strings
            return true;
        }
        if(type2 == "WE_UTE_Vector"){
            //Allow Vector Connections to WE_UTE_Vector types
            VectorInstance* vectorInstance = dynamic_cast<VectorInstance*>(this);
            if(vectorInstance){
                return true;
            }
        }
        if(type1 == "WE_UTE_Vector"){
            //Allow Vector Connections from WE_UTE_Vector types to Vectors.
            VectorInstance* vectorInstance = dynamic_cast<VectorInstance*>(node);
            if(vectorInstance){
                return true;
            }
        }

        if(type1 == "WE_UTE_VariableArguments" || type2 == "WE_UTE_VariableArguments"){
            //Allow anything to connect into the Variable Argument Type.
            return true;
        }
    }
    return false;

}

bool DataNode::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
    case Edge::EC_DATA:{
        if(!dst->isNodeOfType(NT_DATA)){
            //Cannot connect to a non DataNode type.
            return false;
        }
        DataNode* dataNode = (DataNode*) dst;

        if(!isDataProducer()){
            qCritical() << "Cannot connect from something which can't produce";
            //Cannot connect from something which can't produce
            return false;
        }
        if(!dataNode->isDataReciever()){
            qCritical() << "Cannot connect to something which can't recieve";
            //Cannot connect to something which can't recieve
            return false;
        }

        if(dataNode->hasInputData()){
            qCritical() << "Cannot have multiple input datas";
            //Cannot have multiple input datas.
            return false;
        }







        int heightToAncestor = getDepthFromCommonAncestor(dst);
        int heightToComponentImpl = getDepthFromAspect() - 1;

        if(heightToAncestor > heightToComponentImpl){
            //Cannot connect to something outside of the same Component.
            return false;
        }

        Node* sharedAncestor = getParentNode(heightToAncestor);
        if(sharedAncestor){
            if(sharedAncestor->getNodeKind() == NK_AGGREGATE_INSTANCE){
                //Can't data connect if our shared parent is an Aggregate Instance.
                return false;
            }
        }

        //Compare types!
        if(!comparableTypes(dataNode)){
            return false;
        }
        break;
        }
    default:
        break;
    }
    return Node::canAcceptEdge(edgeKind, dst);
}

