#include "tempentity.h"
#include "Entities/data.h"

int str_to_int(QString str){
    bool ok;
    int int_val = str.toInt(&ok, 10);

    if(ok){
        return int_val;
    }
    return -1;
}

TempEntity::TempEntity(GRAPHML_KIND kind, TempEntity* parent)
{
    
    this->parent = parent;
    this->graphml_kind = kind;
    
    if(parent){
        parent->addChild(this);
    }
}

TempEntity::~TempEntity()
{
    clearData();
}

bool TempEntity::isNode()
{
    return graphml_kind == GRAPHML_KIND::NODE;
}

bool TempEntity::isEdge()
{
    return graphml_kind == GRAPHML_KIND::EDGE;
}

int TempEntity::getLineNumber()
{
    return line_number;
}

void TempEntity::setLineNumber(int line_number)
{
    this->line_number = line_number;
}

TempEntity *TempEntity::getParent()
{
    return parent;
}

QString TempEntity::getKind()
{
    return kind_str;
}

void TempEntity::setIDStr(QString id_str){
    this->id_str = id_str;
    //Try and convert the id_str to int, -1 on fail
    this->previous_id = str_to_int(id_str);
}

int TempEntity::getSourceIDInt(){
    return str_to_int(source_id_str);
}

int TempEntity::getTargetIDInt(){
    return str_to_int(target_id_str);
}

bool TempEntity::gotPreviousID()
{
    return previous_id != -1;
}

int TempEntity::getPreviousID()
{
    return previous_id;
}


void TempEntity::setID(int id){
    actual_id = id;
}

int TempEntity::getID(){
    return actual_id;
}

bool TempEntity::gotID(){
    return actual_id != -1;
}

QList<TempEntity*> TempEntity::getChildren(){
    return children;
}

void TempEntity::addChild(TempEntity* child){
    children.push_back(child);
}

void TempEntity::setSourceID(int id)
{
    source_id = id;
}

int TempEntity::getSourceID()
{
    return source_id;
}

void TempEntity::setTargetID(int id)
{
    target_id = id;
}

int TempEntity::getTargetID()
{
    return target_id;
}

void TempEntity::setSourceIDStr(QString id)
{
    source_id_str = id;
}

QString TempEntity::getSourceIDStr()
{
    return source_id_str;
}

void TempEntity::setTargetIDStr(QString id)
{
    target_id_str = id;
}

QString TempEntity::getTargetIDStr()
{
    return target_id_str;
}

void TempEntity::appendEdgeKinds(QList<EDGE_KIND> kinds){
    for(auto kind: kinds){
        appendEdgeKind(kind);
    }
}

void TempEntity::appendEdgeKind(EDGE_KIND kind)
{
    if(!edge_kinds.contains(kind)){
        edge_kinds.append(kind);
    }
}

void TempEntity::removeEdgeKind(EDGE_KIND kind)
{
    edge_kinds.removeAll(kind);
}

EDGE_KIND TempEntity::getEdgeKind()
{
    return edge_kinds.first();
}

bool TempEntity::gotEdgeKind()
{
    return !edge_kinds.isEmpty();
}

QVariant TempEntity::getDataValue(QString key_name){
    QVariant value;
    for(auto data : data_list){
        if(data->getKeyName() == key_name){
            value = data->getValue();
            break;
        }
    }
    return value;
}

bool TempEntity::gotData(QString key_name){
    for(auto data: data_list){
        if(data->getKeyName() == key_name){
            return true;
        }
    }
    return false;
}

void TempEntity::addData(Data *data)
{
    if(data){
        QString key_name = data->getKeyName();
        if(key_name == "kind"){
            kind_str = data->getValue().toString();
        }
        data_list.append(data);
    }
}

QList<Data *> TempEntity::getData()
{
    return data_list;
}

QList<Data *> TempEntity::takeDataList()
{
    QList<Data*> data;
    data.swap(data_list);
    return data;
}

void TempEntity::clearData()
{
    for(auto data : data_list){
        if(!data->getParent()){
            delete data;
        }
    }
    data_list.clear();
}