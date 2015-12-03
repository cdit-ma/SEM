#include "graphmldata.h"
#include <QDebug>

GraphMLData *GraphMLData::clone(GraphMLData *data)
{
    GraphMLData* cloneData = 0;
    if(data){
        GraphMLKey* key = data->getKey();
        QString value = data->getValue();
        cloneData = new GraphMLData(key, value);
    }
    return cloneData;
}

GraphMLData::GraphMLData(GraphMLKey *key, QString value, bool isProtected):GraphML(GraphML::DATA)
{
    //Set to default.
    parentData = 0;
    Parent = 0;
    hasNumber = false;
    numberValue = 0;
    this->key = key;
    if(value == ""){
        if(key->isNumber() && !key->isBoolean()){
            bool isDouble = false;
            qreal newValue = key->getDefaultValue().toDouble(&isDouble);
            if(isDouble){
                numberValue = newValue;
            }
            hasNumber = true;
        }else{
            setValue(key->getDefaultValue());
        }
    }else{
        if(key->isNumber() && !key->isBoolean()){
            bool isDouble = false;
            qreal newValue = value.toDouble(&isDouble);
            if(isDouble){
                numberValue = newValue;
            }
            hasNumber = true;
        }else{
            setValue(value);
        }
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
    if(gotDoubleValue() && !gotBoolValue()){
        return QString::number(numberValue);
    }else{
        return this->value;
    }
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

    QString data;

    QString dataTo = getValue();
    dataTo.replace( "&", "&amp;" );
    dataTo.replace( ">", "&gt;" );
    dataTo.replace( "<", "&lt;" );
    dataTo.replace( "\"", "&quot;" );
    dataTo.replace( "\'", "&apos;" );


    if(hasNumber && !gotBoolValue()){
        dataTo = QString::number(getDoubleValue());
    }
    if(gotBoolValue()){
        if(getBoolValue()){
            dataTo = "true";
        }else{
            dataTo = "false";
        }
    }

    QString returnable = tabSpace + QString("<data key=\"%1\">%2</data>\n").arg(QString::number(getKey()->getID()), dataTo);
    return returnable;
}

QString GraphMLData::toString()
{
    return QString("[%1]GraphMLData: " + this->getKeyName() + ": " + this->getValue()).arg(getID());
}

QStringList GraphMLData::getValidValues(QString kind)
{
    return getKey()->getSelectableValues(kind);
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

void GraphMLData::updateDouble()
{
    numberValue = value.toDouble(&hasNumber);
}

void GraphMLData::setValue(QString newValue)
{
    if(key){
       if(newValue != value){

            QString updatedValue = key->validateDataChange(this, newValue);

            if(updatedValue != value){
                value = updatedValue;
                //Recalculate val.

                if(gotBoolValue()){
                    numberValue = value == "true";
                    hasNumber = true;
                }

                foreach(GraphMLData* data, childData){
                    data->setValue(value);
                }
            }
            emit dataChanged(this);

            if(!gotBoolValue()){
                emit valueChanged(value, keyName);
            }
        }
    }
}

void GraphMLData::setValue(qreal value)
{
    if(key){
       if(value != numberValue){
            hasNumber = true;
            numberValue = value;
            foreach(GraphMLData* data, childData){
                data->setValue(value);
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

bool GraphMLData::compare(GraphMLData *data)
{
    if(data){
        return getValue() == data->getValue();
    }
    return false;
}

void GraphMLData::setParentData(GraphMLData *data, bool protect)
{
    if(data){
        unsetParentData();
        if(data){
            setValue(data->getValue());
        }
    }   
    parentData = data;


    setProtected(protect);
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

void GraphMLData::bindData(GraphMLData *data, bool protect)
{
    if(data && !childData.contains(data)){
        childData.append(data);
        data->setParentData(this, protect);
    }
}

void GraphMLData::unbindData(GraphMLData *data, bool protect)
{
    int index = childData.indexOf(data);
    if(index != -1){
        if(data){
            childData.removeAt(index);
            data->setParentData(0, protect);
        }
    }

}

QList<GraphMLData *> GraphMLData::getBoundData()
{
    return childData;
}

bool GraphMLData::gotDoubleValue() const
{
    return hasNumber;
}

qreal GraphMLData::getDoubleValue() const
{
    return this->numberValue;
}

int GraphMLData::getIntValue() const
{
    return this->numberValue;
}

bool GraphMLData::gotIntValue() const
{
    if(key){
        return key->isInteger();
    }
}

bool GraphMLData::getBoolValue() const
{
    return this->numberValue;
}

bool GraphMLData::gotBoolValue() const
{
    if(key){
        return key->isBoolean();
    }
}
