#include "graphmlkey.h"
#include <QDebug>

int GraphMLKey::_Did =0;

GraphMLKey::GraphMLKey(QString name, QString typeStr, QString forStr):GraphML(GraphML::KEY, name)
{
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
        this->forKind = GraphML::GRAPH;
    }else if(forStr ==  QString("node")){
        this->forKind = GraphML::NODE;
    }else if(forStr == QString("edge")){
        this->forKind = GraphML::EDGE;
    }else if(forStr == QString("all")){
        this->forKind = GraphML::ALL;
    }else{
        qDebug() << "Attribute forType:" << forStr << "Not implemented";
    }

    this->typeStr = typeStr;
    this->forKindStr = forStr;
    this->defaultValue = "";

    setDefaultProtected(false);

    //this->type = type;
}

GraphMLKey::~GraphMLKey()
{

}

void GraphMLKey::setDefaultProtected(bool setProtected)
{
    protectedKey = setProtected;

}

bool GraphMLKey::isProtected()
{
    return protectedKey;
}

void GraphMLKey::setDefaultValue(QString value)
{
    defaultValue = value;
}

QString GraphMLKey::getDefaultValue() const
{
    return this->defaultValue;
}

bool GraphMLKey::equals(GraphMLKey *key)
{
    if(getType() != key->getType()){
        return false;
    }
    if(getName() != key->getName()){
        return false;
    }
    if(getForKind() != key->getForKind()){
        return false;
    }
    return true;
}


GraphML::KIND GraphMLKey::getForKind() const
{
    return this->forKind;
}

QString GraphMLKey::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }

    QString returnable = tabSpace + QString("<key attr.name=\"%1\" attr.type=\"%2\" for=\"%3\" id=\"%4\"").arg(this->getName(),this->typeStr,this->forKindStr,this->getID());
    if(this->getDefaultValue() != ""){
        returnable += ">\n";
        returnable += tabSpace + QString("\t<default>%1</default>\n").arg(this->getDefaultValue());
        returnable += tabSpace + "</key>\n";
    }else{
        returnable += "/>\n";
    }
    return returnable;
}

GraphMLKey::TYPE GraphMLKey::getType() const
{
    return this->type;
}

QString GraphMLKey::getTypeString()
{
    return this->typeStr;
}

QString GraphMLKey::getForKindString()
{
    return this->forKindStr;
}


QString GraphMLKey::toString()
{
     return QString("Key[%1]: "+this->getName()).arg(this->getID());

}
