#include "graphmlkey.h"
#include "graphmldata.h"
#include "node.h"

#include <QDebug>
#include <QMessageBox>
int GraphMLKey::_Did =0;

GraphMLKey::GraphMLKey(QString name, QString typeStr, QString forStr):GraphML(GraphML::KEY, name)
{
    //Parse Type
    if(typeStr == QString("boolean")){
        this->type = BOOLEAN;
        this->defaultValue = "false";
    }else if(typeStr == QString("string")){
        this->type = STRING;
        this->defaultValue = "";
    }else if(typeStr == QString("double")){
        this->type = DOUBLE;
        this->defaultValue = "0.0";
    }else if(typeStr == QString("int")){
        this->type = INT;
        this->defaultValue = "0";
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

    invalidLabelCharacters << '*' << '.' << '[' << ']'<< ';' << '|' << ',' <<  '%';
    //invalidLabelCharacters << '"' << '/' << '\\' << '=' << ':' << ' ';
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

void GraphMLKey::appendValidValues(QStringList values, QStringList nodeKinds)
{
    foreach(QString nodeKind, nodeKinds){
        foreach(QString value, values){
            addValidValue(nodeKind, value);
        }
    }
}

void GraphMLKey::appendValidValues(QStringList values, QString nodeKind)
{
    foreach(QString value, values){
        addValidValue(nodeKind, value);
    }
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

bool GraphMLKey::gotSelectableValues(QString nodeKind)
{
    QString ALL("ALL");

    if(validValues.contains(nodeKind)){
        return true;
    }

    return validValues.contains(ALL);
}

QStringList GraphMLKey::getSelectableValues(QString nodeKind)
{
    QString ALL("ALL");
    QStringList vValues;
    if(validValues.contains(nodeKind)){
        vValues = validValues.values(nodeKind);
    }else if(validValues.contains(ALL)){
        vValues = validValues.values(ALL);
    }
    vValues.sort();
    return vValues;
}

QString GraphMLKey::validateDataChange(GraphMLData *data, QString newValue)
{

    bool ok = false;


    switch(type){
    case BOOLEAN:
        newValue = newValue.toLower();
        if(newValue == "true" || newValue == "false"){
            return newValue;
        }
        break;
    case INT:
        newValue.toInt(&ok);
        break;
    case LONG:
        newValue.toLong(&ok);
    case DOUBLE:{
        double temp = newValue.toDouble(&ok);
        if((getName() == "width" || getName() == "height") && temp  <= 0){
            ok = false;
        }

        break;
    }
    case FLOAT:
        newValue.toFloat(&ok);
        break;
    case STRING:{
        ok = true;
        if(getName() == "label"){
            if(newValue.size() > 64)
            {
                QMessageBox::warning(0, "Label Warning!", "Label is longer than 32 characters!", QMessageBox::Ok);
            }

            //newValue.replace(QString(" "), QString("_"));
            foreach(QChar letter, newValue){

                if(invalidLabelCharacters.contains(letter)){
                    QMessageBox::critical(0, "Invalid Character in Label!", "Character '" + QString(letter) + "' is not allowed in label!", QMessageBox::Ok);
                    ok = false;
                }
            }
       }

        break;
    }
    default:
#ifdef DEBUG_MODE
        qWarning() << "Cannot Validate Data Change";
#endif
        ok = true;
    }

    if(ok){


        return newValue;
    }else{
        return data->getValue();
    }
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

void GraphMLKey::addValidValue(QString nodeKind, QString value)
{
    if(!validValues.contains(nodeKind,value)){
        validValues.insert(nodeKind, value);
    }
}


QString GraphMLKey::toString()
{
    return QString("[%1]GraphMLKey").arg(getID());
}
