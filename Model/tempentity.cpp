#include "tempentity.h"

TempEntity::TempEntity(Entity::ENTITY_KIND entityKind, TempEntity *parent)
{
    lineNumber = -1;
    actualID = -1;
    actualParentID = -1;
    this->parent = parent;

    readOnlyState.snippetID = -1;
    readOnlyState.snippetMAC = -1;
    readOnlyState.snippetTime = -1;
    isReadOnly = false;

    this->entityKind = entityKind;
}

void TempEntity::setLineNumber(int lineNumber)
{
    this->lineNumber = lineNumber;
}

bool TempEntity::isTop()
{
    return parent == 0;
}

void TempEntity::setID(QString ID)
{
    this->ID = ID;
}

void TempEntity::setActualID(int ID)
{
    actualID = ID;
}

TempEntity *TempEntity::getParentEntity()
{

    return parent;
}

void TempEntity::setParentID(QString ID)
{
    this->parentID = ID;
}

void TempEntity::setActualParentID(int ID)
{
    actualParentID = ID;
}

QString TempEntity::getParentID()
{
    return parentID;
}

int TempEntity::getActualParentID()
{
    return actualParentID;
}

bool TempEntity::gotActualID()
{
    return actualID > 0;
}

bool TempEntity::gotActualParentID()
{
    return actualParentID > 0;
}

ReadOnlyState TempEntity::getReadOnlyState()
{
    return readOnlyState;
}

bool TempEntity::gotReadOnlyState()
{
    return readOnlyState.isValid();
}

void TempEntity::addData(Data *data)
{
    if(data){
        QString keyName = data->getKeyName();
        if(keyName == "readOnly"){
            isReadOnly = data->getValue().toBool();
        }else if(keyName == "snippetID"){
            readOnlyState.snippetID = data->getValue().toInt();
        }else if(keyName == "snippetTime"){
            readOnlyState.snippetTime = data->getValue().toInt();
        }else if(keyName == "snippetMAC"){
            readOnlyState.snippetMAC = data->getValue().toLongLong();
        }
        dataList.append(data);
    }
}

QList<Data *> TempEntity::getData()
{
    return dataList;
}

QString TempEntity::getOriginalID()
{
    return ID;
}

int TempEntity::getActualID()
{
    return actualID;
}

bool TempEntity::isNode()
{
    return entityKind == Entity::EK_NODE;
}

bool TempEntity::isEdge()
{
    return entityKind == Entity::EK_EDGE;
}

void TempEntity::setSrcID(QString ID)
{
    srcID = ID;
}

void TempEntity::setDstID(QString ID)
{
    dstID = ID;
}

QString TempEntity::getSrcID()
{
    return srcID;
}

QString TempEntity::getDstID()
{
    return dstID;
}
