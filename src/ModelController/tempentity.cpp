#include "tempentity.h"
#include "Entities/data.h"

TempEntity::TempEntity(GRAPHML_KIND entityKind, TempEntity *parent)
{
    lineNumber = -1;
    actualID = -1;
    oldID = -1;
    actualSrcID = -1;
    actualDstID = -1;
    src = 0;
    dst = 0;

    actualParentID = -1;
    ignoreConstruction = false;
    _resetPosition = false;
    nodeKind = "";
    this->parent = parent;

    readOnlyState.snippetID = -1;
    readOnlyState.snippetMAC = -1;
    readOnlyState.snippetTime = -1;
    readOnlyState.exportTime = -1;
    readOnlyState.isDefinition = false;
    isReadOnly = false;

    retryCount = 0;
    this->entityKind = entityKind;
}

TempEntity::~TempEntity()
{
    while(!dataList.isEmpty()){
        Data* data = dataList.takeFirst();
        if(!data->getParent()){
            delete data;
        }
    }
}

void TempEntity::setLineNumber(int lineNumber)
{
    this->lineNumber = lineNumber;
}

void TempEntity::setResetPosition()
{
    _resetPosition = true;
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

void TempEntity::resetIncrementCount()
{
    retryCount = 0;
}

void TempEntity::setSource(Node *src)
{
    this->src = src;
}

void TempEntity::setDestination(Node *dst)
{
    this->dst = dst;

}

Node *TempEntity::getSource()
{
    return src;
}

Node *TempEntity::getDestination()
{
    return dst;

}

void TempEntity::setID(QString ID)
{
    this->ID = ID;
}

void TempEntity::setPrevID(int ID)
{
    oldID = ID;
}

bool TempEntity::hasPrevID()
{
    return oldID > 0;
}

int TempEntity::getPrevID()
{
    return oldID;
}

void TempEntity::setActualID(int ID)
{
    actualID = ID;
}

void TempEntity::appendEdgeKind(EDGE_KIND edgeKind)
{
    if(!edgeKinds.contains(edgeKind)){
        edgeKinds.append(edgeKind);
    }
}

void TempEntity::removeEdgeKind(EDGE_KIND edgeKind)
{
    edgeKinds.removeAll(edgeKind);
}

EDGE_KIND TempEntity::getEdgeKind()
{
    return edgeKinds.first();
}

bool TempEntity::hasEdgeKind()
{
    return !edgeKinds.isEmpty();
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

QString TempEntity::getKind()
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

QVariant TempEntity::getData(QString key){

    QVariant val;
    for(auto data: dataList){
        if(data->getKeyName() == key){
            val = data->getValue();
            break;
        }
    }
    return val;
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
            readOnlyState.snippetTime = data->getValue().toLongLong();
        }else if(keyName == "snippetMAC"){
            readOnlyState.snippetMAC = data->getValue().toLongLong();
        }else if(keyName == "exportTime"){
            readOnlyState.exportTime = data->getValue().toLongLong();
        }else if(keyName == "readOnlyDefinition"){
            readOnlyState.isDefinition = data->getValue().toBool();
        }else if(_resetPosition && (keyName == "x" || keyName == "y")){
            data->setValue(-1);
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
    return entityKind == GRAPHML_KIND::NODE;
}

bool TempEntity::isEdge()
{
    return entityKind == GRAPHML_KIND::EDGE;
}

void TempEntity::setSrcID(QString ID)
{
    srcID = ID;
}

void TempEntity::setDstID(QString ID)
{
    dstID = ID;
}

void TempEntity::setActualSrcID(int ID)
{
    actualSrcID = ID;
}

void TempEntity::setActualDstID(int ID)
{
    actualDstID = ID;
}

QString TempEntity::getSrcID()
{
    return srcID;
}

QString TempEntity::getDstID()
{
    return dstID;
}

int TempEntity::getActualSrcID()
{
    return actualSrcID;
}

int TempEntity::getActualDstID()
{
    return actualDstID;
}
