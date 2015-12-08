#include "parameter.h"
#include "behaviournode.h"
#include "../InterfaceDefinitions/memberinstance.h"
#include "variable.h"
#include "attributeimpl.h"
#include "../InterfaceDefinitions/vectorinstance.h"
#include "../InterfaceDefinitions/aggregateinstance.h"
#include <QDebug>

Parameter::Parameter(bool isInput):BehaviourNode()
{
    addValidEdgeType(Edge::EC_DATA);
    inputParameter = isInput;
}

bool Parameter::isInputParameter()
{
    return inputParameter;
}

bool Parameter::isReturnParameter()
{
    return !inputParameter;
}

bool Parameter::hasConnection()
{
    return hasEdges();
}

bool Parameter::compareableTypes(Node *node)
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
        QString type1 = getDataValue("type");
        QString type2 = node->getDataValue("type");

        if(numberTypes.contains(type1) && numberTypes.contains(type2)){
            return true;
        }
        if(stringTypes.contains(type1) && stringTypes.contains(type2)){
            return true;
        }
    }
    return false;
}

bool Parameter::canAdoptChild(Node*)
{
    return false;
}

bool Parameter::canConnect_DataEdge(Node *node)
{
    return BehaviourNode::canConnect_DataEdge(node);

}
