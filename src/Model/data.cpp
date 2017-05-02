#include "data.h"
#include <QDebug>

Data::Data(Key *key, QVariant value, bool protect):GraphML(GK_DATA)
{
    _parent = 0;
    _parentDataID = -1;
    _parentData = 0;
    _key = key;
    _isProtected = false;
    _isDataLinked = false;
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
    //setParent(0);

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
    if(_parent){
        _parent->_dataRemoved(this);
    }

    if(parent){
        setID();
        parent->_dataChanged(this);
        parent->_dataProtected(this);
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
    updateProtected();
}

void Data::updateProtected(){
    if(getParent()){
        _parent->_dataProtected(this);
    }
}

bool Data::isProtected() const
{
    return _isDataLinked || _isProtected;
}

bool Data::_setValue(QVariant value, bool validate){
    //Set using the 
    QVariant new_value = value;
    
    if(validate && _key){
        new_value = _key->validateDataChange(this, new_value);
    }

    //Check if the data changed
    bool data_changed = new_value != _value;
    _value = new_value;

    updateChildren(data_changed);
    return data_changed;
}


bool Data::forceValue(QVariant value){
    return _setValue(value, false);
}

bool Data::setValue(QVariant value)
{
    return _setValue(value, true);
}

void Data::setParentData(Data *parentData)
{
    unsetParentData();

    if(parentData){
        parentData->addChildData(this);
        _parentData = parentData;
        _parentDataID = parentData->getID();
        _isDataLinked = true;
        updateProtected();
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
        _isDataLinked = false;
        //Update to use the parents protected status.
        updateProtected();
    }
}

void Data::clearValue()
{
    _value = QVariant();
    updateChildren();
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
    return false;
}


Key *Data::getKey()
{
    return _key;
}

QString Data::getKeyName() const
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

    if(getKey()->getName() == "processes_to_log"){
        dataString.replace("\n", ",");
    }


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
        //Update.
        childData->forceValue(getValue());
    }
}

void Data::removeChildData(Data *childData)
{
    if(childData){
        int ID = childData->getID();
        if(_childData.contains(ID)){
            _childData.remove(ID);
        }
        childData->clearValue();
    }
}

void Data::revalidateData(){
    setValue(getValue());
}

void Data::parentDataChanged(int ID, QString, QVariant data)
{
    if(ID == _parentDataID){
        //If this signal is coming from our parent, update our value
        setValue(data);
    }
}

void Data::updateChildren(bool changed)
{
    //Send a signal saying the data changed, regardless of whether it did.
    if(changed){
        foreach(Data* data, _childData.values()){
            data->forceValue(getValue());
        }
    }
    if(getParent()){
        _parent->_dataChanged(this);
    }
    emit dataChanged(getValue());
}
