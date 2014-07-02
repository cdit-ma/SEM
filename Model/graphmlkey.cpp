#include "graphmlkey.h"
#include <QDebug>

int GraphMLKey::_Did =0;

GraphMLKey::GraphMLKey(QString name, QString typeStr, QString forStr):GraphML(GraphML::KEY, name)
{
     this->setID(QString("d%1").arg(this->_Did++));
    //Parse Type
    if(typeStr == QString("boolean")){
        this->type = BOOLEAN;
    }else if(typeStr == QString("string")){
        this->type = STRING;
    }else if(typeStr == QString("double")){
        this->type = DOUBLE;
    }else{
        qDebug() << "Attribute type:" << typeStr << "Not implemented";
    }

    if(forStr == QString("graph")){
        this->kind = GraphML::GRAPH;
    }else if(forStr ==  QString("node")){
        this->kind = GraphML::NODE;
    }else{
        qDebug() << "Attribute forType:" << forStr << "Not implemented";
    }

    this->typeStr = typeStr;
    this->kindStr = forStr;

    this->kind = kind;
    this->type = type;
}

bool GraphMLKey::operator==(const GraphMLKey &other) const
{
    if(this->getType() != other.getType()){
        return false;
    }
    if(this->getName() != other.getName()){
        return false;
    }
    if(this->getKind() != other.getKind()){
        return false;
    }
    return true;
}

QString GraphMLKey::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }

    QString returnable = tabSpace + QString("<key attr.name=\"%1\" attr.type=\"%2\" for=\"%3\" id=\"%4\"/>\n").arg(this->getName(),this->typeStr,this->kindStr,this->getID());
    return returnable;
}

GraphMLKey::TYPE GraphMLKey::getType() const
{
    return this->type;
}

GraphML::KIND GraphMLKey::getKind() const
{
    return this->kind;
}

QString GraphMLKey::toString()
{
     return QString("Key[%1]: "+this->getName()).arg(this->getID());

}
