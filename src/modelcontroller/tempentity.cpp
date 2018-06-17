#include "tempentity.h"
#include "entityfactory.h"

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
}

bool TempEntity::isNode() const
{
    return graphml_kind == GRAPHML_KIND::NODE;
}

bool TempEntity::isEdge() const
{
    return graphml_kind == GRAPHML_KIND::EDGE;
}

int TempEntity::getLineNumber()
{
    return line_number;
}

NODE_KIND TempEntity::getNodeKind(){
    if(isNode() && node_kind == NODE_KIND::NONE){
        auto data = getDataValue("kind").toString();
        node_kind = EntityFactory::getNodeKind(data);
    }
    return node_kind;
}

const QList<NODE_KIND>& TempEntity::getParentStack(){
    return parent_stack;
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

QString TempEntity::getIDStr(){
    return id_str;
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
    if(parent){
        child->parent_stack = parent_stack;
        if(isNode()){
            child->parent_stack += getNodeKind();
        }
    }
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

int TempEntity::getTargetID() const
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
    return data.value(key_name, QVariant());
}

bool TempEntity::gotData(QString key_name){
    return data.contains(key_name);
}

void TempEntity::addData(QString key_name, QVariant value){
    data[key_name] = value;
    if(key_name == "kind"){
        kind_str = value.toString();
    }
}

void TempEntity::removeData(QString key_name){
    data.remove(key_name);
}

void TempEntity::clearData(){
    data.clear();
}

QList<QString> TempEntity::getKeys(){
    return data.keys();
}

void TempEntity::AddImplicitlyConstructedNodeID(NODE_KIND kind, int id){
    implicit_nodes[kind].enqueue(id);
}
void TempEntity::AddImplicitlyConstructedEdgeID(EDGE_KIND kind, int id){
    implicit_edges[kind].enqueue(id);
}


bool TempEntity::GotImplicitlyConstructedNodeID(NODE_KIND kind){
    return implicit_nodes[kind].count() > 0;
}

bool TempEntity::GotImplicitlyConstructedEdgeID(EDGE_KIND kind){
    return implicit_edges[kind].count() > 0;
}

int TempEntity::TakeNextImplicitlyConstructedNodeID(NODE_KIND kind){
    return implicit_nodes[kind].dequeue();
}
int TempEntity::TakeNextImplicitlyConstructedEdgeID(EDGE_KIND kind){
    return implicit_edges[kind].dequeue();
}

void TempEntity::setImplicitlyConstructed(){
    implicitly_constructed = true;
}
bool TempEntity::isImplicitlyConstructed(){
    return implicitly_constructed;
}

QQueue<int> TempEntity::GetLeftOverImplicitlyConstructedNodeIds(){
    QQueue<int> values;
    for(auto queue : implicit_nodes.values()){
        values += queue;
    }
    return values;
}   

bool TempEntity::isUUIDMatched(){
    return uuid_matched;
}
void TempEntity::setUUIDMatched(bool uuid_matched){
    this->uuid_matched = uuid_matched;
}