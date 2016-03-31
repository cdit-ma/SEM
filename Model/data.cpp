#include "data.h"
#include <QDebug>

Data::Data(Key *key, QVariant value, bool protect):GraphML(GK_DATA)
{
    _parent = 0;
    _parentDataID = -1;
    _parentData = 0;
    _key = key;
    _isProtected = false;
    if(key){
        _keyName = key->getName();
        setProtected(key->isProtected());
    }

    if(value.isValid()){
        setValue(value);
    }
    if(protect){
        setProtected(protect);
    }
}

Data::~Data()
{
    //Unset the parent
    setParent(0);

    if(_parentData){
        //Unset Parent Data.
        _parentData->removeChildData(this);
    }
    QList<Data*> childData = _childData.values();
    while(!childData.isEmpty()){
        Data* cData = childData.takeFirst();
        if(cData){
            cData->unsetParentData();
        }
    }
}

Data *Data::clone(Data *data)
{
    Data* cloneData = 0;
   if(data){
       Key* key = data->getKey();
       QVariant value = data->getValue();
       cloneData = new Data(key);
       cloneData->setValue(value);
       cloneData->setProtected(data->isProtected());
}
   return cloneData;

}

void Data::setParent(Entity *parent)
{
    if(parent){
        connect(this, SIGNAL(dataChanged(int,QString,QVariant)), parent, SLOT(dataChanged(int, QString,QVariant)));
        //Set the ID
        setID();
    }
    if(_parent){
        disconnect(this, SIGNAL(dataChanged(int,QString,QVariant)), _parent, SLOT(dataChanged(int, QString,QVariant)));
    }


    _parent = parent;
}

Entity *Data::getParent()
{
    return _parent;
}

void Data::setProtected(bool protect)
{
    _isProtected = protect;
}

bool Data::isProtected() const
{
    return _isProtected;
}


bool Data::setValue(QVariant value)
{
    QVariant newValue = value;
    if(_key){
        newValue = _key->validateDataChange(this, value);
    }

    bool _dataChanged = false;
    if(newValue.isValid() && newValue != _value){
        _dataChanged = true;
        _value = newValue;
    }

    //Send a signal saying the data changed, regardless of whether it did.
    emit dataChanged(getID(), getKeyName(), _value);
    return _dataChanged;
}

void Data::setParentData(Data *parentData, bool protect)
{
    unsetParentData();


    if(parentData){
        parentData->addChildData(this);
        _parentData = parentData;
        _parentDataID = parentData->getID();
        setProtected(protect);
    }
}

Data *Data::getParentData()
{
    return _parentData;
}

void Data::unsetParentData()
{
    if(_parentData){
        _parentData->removeChildData(this);
        _parentData = 0;
        _parentDataID = -1;
        //Update to use the parents protected status.
        setProtected(_key->isProtected());
    }
}

void Data::clearValue()
{
    _value = QVariant();
    emit dataChanged(getID(), getKeyName(), _value);
}

bool Data::compare(const Data *data) const
{
    if(data){
        return _value == data->getValue();
    }
    return false;
}

QList<Data *> Data::getChildData()
{
    return _childData.values();
}

bool Data::isVisualData()
{
    if(_key){
        return _key->isVisualData();
    }
    return false;
}


Key *Data::getKey()
{
    return _key;
}

QString Data::getKeyName()
{
    return _keyName;
}

QVariant Data::getValue() const
{
    return _value;
}

QString Data::toGraphML(int indentDepth)
{
    QString tabSpace;
    tabSpace.fill('\t', indentDepth);

    QString dataString = _value.toString();

    dataString.replace( "&", "&amp;" );
    dataString.replace( ">", "&gt;" );
    dataString.replace( "<", "&lt;" );
    dataString.replace( "\"", "&quot;" );
    dataString.replace( "\'", "&apos;" );


    QString xml;
    xml += tabSpace;
    xml += QString("<data key=\"%1\">%2</data>\n").arg(QString::number(getKey()->getID()), dataString);
    return xml;
}

QString Data::toString()
{
    return QString("[" + QString::number(getID()) + "] Data " + getKeyName() + ": " + getValue().toString());
}

void Data::addChildData(Data *childData)
{
    if(childData){
        int ID = childData->getID();
        if(!_childData.contains(ID)){
            _childData[ID] = childData;
        }
        connect(this, SIGNAL(dataChanged(int,QString,QVariant)), childData, SLOT(parentDataChanged(int, QString, QVariant)));
        //Update.
        childData->setValue(getValue());
    }
}

void Data::removeChildData(Data *childData)
{
    if(childData){
        int ID = childData->getID();
        if(_childData.contains(ID)){
            _childData.remove(ID);
        }
        disconnect(this, SIGNAL(dataChanged(int, QString, QVariant)), childData, SLOT(parentDataChanged(int, QString, QVariant)));
        childData->clearValue();
    }
}

void Data::parentDataChanged(int ID, QString, QVariant data)
{
    if(ID == _parentDataID){
        //If this signal is coming from our parent, update our value
        setValue(data);
    }
}
