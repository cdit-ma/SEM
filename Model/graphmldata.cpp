#include "graphmldata.h"
#include <QDebug>

GraphMLData::GraphMLData(GraphMLKey *key, QString value):GraphML(GraphML::DATA)
{
    this->key = key;
    if(value==""){
        this->setValue(this->key->getDefaultValue());
    }else{
        this->setValue(value);
    }

    //Set to default.
    parentData = 0;
    this->setProtected(key->getProtected());
}

GraphMLData::~GraphMLData()
{
    unsetParentData();

    for(int i = 0; i < childData.size();i++){
        GraphMLData* child = childData[i];
        unbindData(child);
    }

}
/*
void GraphMLData::setValue(QString value)
{
    this->value = value;
}
*/

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
    if(key){
        return key->getName();
    }
    return "";
}

QString GraphMLData::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }

    QString returnable = tabSpace + QString("<data key=\"%1\">%2</data>\n").arg(this->getKey()->getID(), this->getValue());
    return returnable;
}

QString GraphMLData::toString()
{
    return QString("Data[%1]: "+this->getName()).arg(this->getID());
}

QStringList GraphMLData::toStringList()
{
     QStringList dataDump;
     dataDump << key->getName();
     dataDump << key->getTypeString();
     dataDump << key->getForKindString();
     dataDump << this->getValue();
     return dataDump;

}

void GraphMLData::setValue(QString value)
{
    if(value != this->value){
        this->value = value;
        emit dataChanged(this);
    }

    foreach(GraphMLData* data, childData){
        data->setValue(value);
    }
}

void GraphMLData::setProtected(bool setProtected)
{
    isProtected = setProtected;
}

bool GraphMLData::getProtected()
{
    return isProtected;
}

void GraphMLData::setParentData(GraphMLData *data)
{
    if(data){
        unsetParentData();
    }
    parentData = data;

}

void GraphMLData::unsetParentData()
{
    if(parentData != 0){
        qCritical() << "Unbinding";
        parentData->unbindData(this);
        qCritical() << "unBound";
    }
    parentData = 0;
}

void GraphMLData::bindData(GraphMLData *data)
{
    if(!childData.contains(data)){
        childData.append(data);
        //Update Data.
        data->setValue(getValue());

        data->setParentData(this);
    }
}

void GraphMLData::unbindData(GraphMLData *data)
{
    int index = childData.indexOf(data);
    if(index != -1){
        if(data){
            //data->unsetParentData();
            childData.removeAt(index);
            data->setParentData(0);
        }
    }
}
