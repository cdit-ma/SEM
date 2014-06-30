#include "graphmlattribute.h"
#include <QDebug>

GraphMLAttribute::GraphMLAttribute( QString id, QString name, QString typeStr, QString forStr)
{

    //Parse Type
    if(typeStr == QString("boolean")){
        this->type = BOOLEAN;
    }else if(typeStr == QString("string")){
        this->type = STRING;
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
    this->name = name;
    this->id = id;
}

QString GraphMLAttribute::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }

    QString returnable = tabSpace + QString("<key attr.name=\"%1\" attr.type=\"%2\" for=\"%3\" id=\"%4\"/>\n").arg(this->getName(),this->typeStr,this->kindStr,this->getID());
    return returnable;
}

QString GraphMLAttribute::getID() const
{
    return this->id;
}

QString GraphMLAttribute::getName() const
{
    return this ->name;
}

GraphMLAttribute::TYPE GraphMLAttribute::getType() const
{
    return this->type;
}

GraphML::KIND GraphMLAttribute::getKind() const
{
    return this->kind;
}
