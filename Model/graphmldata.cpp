#include "graphmldata.h"
#include <QDebug>

GraphMLData::GraphMLData(GraphMLKey *key, QString value, bool isProtected):GraphML(GraphML::DATA)
{
    //Set to default.
    parentData = 0;
    Parent = 0;
    this->key = key;
    if(value == ""){
        this->setValue(key->getDefaultValue());
    }else{
        this->setValue(value);
    }
    if(key){
        this->keyName = key->getName();
        this->type = key->getTypeString();
    }

    if(isProtected){
        setProtected(isProtected);
    }else{
        setProtected(key->isProtected());
    }
}

GraphMLData::~GraphMLData()
{
    if(this->parentData){
        this->parentData->unbindData(this);
    }
    while(childData.size() > 0){
        GraphMLData* child = childData.takeFirst();
        if(child){
            child->setParentData(0);
        }
    }


}

void GraphMLData::setParent(GraphML *parent)
{
    Parent = parent;
}

GraphML *GraphMLData::getParent()
{
    return Parent;
}

void GraphMLData::clearValue()
{
    setValue("");
}

QString GraphMLData::getValue() const
{
    return this->value;
}

GraphMLKey *GraphMLData::getKey()
{
    return this->key;
}

QString GraphMLData::getKeyName()
{
    return keyName;
}

QString GraphMLData::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }

    QString dataTo = getValue();
    dataTo.replace( "&", "&amp;" );
    dataTo.replace( ">", "&gt;" );
    dataTo.replace( "<", "&lt;" );
    dataTo.replace( "\"", "&quot;" );
    dataTo.replace( "\'", "&apos;" );

    QString returnable = tabSpace + QString("<data key=\"%1\">%2</data>\n").arg(QString::number(getKey()->getID()), dataTo);
    return returnable;
}

QString GraphMLData::toString()
{
    return QString("[%1]GraphMLData: " + this->getKeyName() + ": " + this->getValue()).arg(getID());
}

QStringList GraphMLData::toStringList()
{
     QStringList dataDump;
     dataDump << key->getName();
     dataDump << key->getTypeString();
     dataDump << key->getForKindString();
     dataDump << this->getValue();

     if(this->isProtected()){
         dataDump << "true";
     }else{
         dataDump << "false";
     }
     return dataDump;

}

QList<int> GraphMLData::getBoundIDS()
{
    QList<int> dataDump;
    foreach(GraphMLData* childdata, childData){
        dataDump << childdata->getID();
    }
    return dataDump;
}

void GraphMLData::setValue(QString newValue)
{
    if(key){
       if(newValue != value){

            QString updatedValue = key->validateDataChange(this, newValue);

            if(updatedValue != value){
                value = updatedValue;
                foreach(GraphMLData* data, childData){
                    data->setValue(value);
                }
            }
            emit dataChanged(this);
            emit valueChanged(value, keyName);
        }
    }
}

void GraphMLData::setProtected(bool setProtected)
{
    protectedData = setProtected;
}



bool GraphMLData::isProtected()
{
    return protectedData;
}

void GraphMLData::setParentData(GraphMLData *data)
{
    if(data){
        unsetParentData();
        if(data){
            setValue(data->getValue());
            setProtected(true);
        }
    }
    parentData = data;
}

void GraphMLData::unsetParentData()
{
    if(parentData){
        parentData->unbindData(this);
        setProtected(false);
    }
    parentData = 0;
}

GraphMLData *GraphMLData::getParentData()
{
    return parentData;
}

void GraphMLData::bindData(GraphMLData *data)
{
    if(data && !childData.contains(data)){
        childData.append(data);
        data->setParentData(this);
    }
}

void GraphMLData::unbindData(GraphMLData *data)
{
    int index = childData.indexOf(data);
    if(index != -1){
        if(data){
            childData.removeAt(index);
            data->setParentData(0);
        }
    }

}

QList<GraphMLData *> GraphMLData::getBoundData()
{
    return childData;
}
