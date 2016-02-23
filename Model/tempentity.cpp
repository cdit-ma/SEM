#include "tempentity.h"

TempEntity::TempEntity(Entity::ENTITY_KIND entityKind, TempEntity *parent)
{
    lineNumber = -1;
    actualID = -1;
    actualParentID = -1;
    ignoreConstruction = false;
    nodeKind = "";
    this->parent = parent;

    readOnlyState.snippetID = -1;
    readOnlyState.snippetMAC = -1;
    readOnlyState.snippetTime = -1;
    readOnlyState.exportTime = -1;
    isReadOnly = false;

    retryCount = 0;
    this->entityKind = entityKind;
}

TempEntity::~TempEntity()
{
    if(ignoreConstruction){
        while(!dataList.isEmpty()){
            Data* data = dataList.takeFirst();
            delete data;
        }
    }
}

void TempEntity::setLineNumber(int lineNumber)
{
    this->lineNumber = lineNumber;
}

bool TempEntity::shouldConstruct()
{
    return !ignoreConstruction;
}

bool TempEntity::ignoreConstruct()
{
    return ignoreConstruction;
}

void TempEntity::setIgnoreConstruction(bool ignore)
{
    ignoreConstruction = ignore;
}

bool TempEntity::isTop()
{
    return parent == 0;
}

int TempEntity::getRetryCount()
{
    return retryCount;
}

void TempEntity::incrementRetryCount()
{
    retryCount ++ ;
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

QString TempEntity::getNodeKind()
{
    return nodeKind;
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

int TempEntity::getLineNumber()
{
    return lineNumber;
}

void TempEntity::addData(Data *data)
{
    if(data){
        QString keyName = data->getKeyName();
        if(keyName == "kind"){
            nodeKind = data->getValue().toString();
        }else if(keyName == "readOnly"){
            isReadOnly = data->getValue().toBool();
        }else if(keyName == "snippetID"){
            readOnlyState.snippetID = data->getValue().toInt();
        }else if(keyName == "snippetTime"){
            readOnlyState.snippetTime = data->getValue().toInt();
        }else if(keyName == "snippetMAC"){
            readOnlyState.snippetMAC = data->getValue().toLongLong();
        }else if(keyName == "exportTime"){
            readOnlyState.exportTime = data->getValue().toInt();
        }
        dataList.append(data);
    }
}

QList<Data *> TempEntity::getData()
{
    return dataList;
}

QList<Data *> TempEntity::takeDataList()
{
    QList<Data*> data = dataList;
    dataList.clear();
    return data;
}

void TempEntity::clearData()
{
    while(!dataList.isEmpty()){
        Data* data = dataList.takeFirst();
        if(data){
            if(!data->getParent()){
                delete data;
            }
        }
    }
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
