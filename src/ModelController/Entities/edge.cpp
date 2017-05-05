#include "edge.h"
#include "node.h"
#include "data.h"
#include <QDebug>

#include "../entityfactory.h"
#include "../edgekinds.h"

void Edge::RegisterEdgeKind(EntityFactory* factory, EDGE_KIND kind, QString kind_string, std::function<Edge* (Node*, Node*)> constructor){
    if(factory){
        factory->RegisterEdgeKind(kind, kind_string, constructor);
    }
}
void Edge::RegisterDefaultData(EntityFactory* factory, EDGE_KIND kind, QString key_name, QVariant::Type type, bool is_protected, QVariant value){
    if(factory){
        factory->RegisterDefaultData(kind, key_name, type, is_protected, value);
    }
}

Edge::Edge(Node *source, Node *destination, EDGE_KIND kind):Entity(GRAPHML_KIND::EDGE)
{
    //Set the instance Variables
    this->kind = kind;
    this->source = source;
    this->destination = destination;

    //Attach the Edge to its source/Destination
    if(source && destination){
        source->addEdge(this);
        destination->addEdge(this);
    }
}

EDGE_KIND Edge::getEdgeKind() const{
    return kind;    
}

Edge::Edge(EntityFactory* factory, EDGE_KIND kind, QString kind_string):Entity(GRAPHML_KIND::EDGE){
    RegisterDefaultData(factory, kind, "kind", QVariant::String, true, kind_string);
}

Edge::~Edge()
{
    if(destination){
        destination->removeEdge(this);
    }
    if(source){
        source->removeEdge(this);
    }
}

Node *Edge::getSource() const
{
    return source;
}

int Edge::getSourceID() const
{
    int id = -1;
    if(source){
        id = source->getID();
    }
    return id;
}

int Edge::getDestinationID() const
{
    int id = -1;
    if(destination){
        id = destination->getID();
    }
    return id;
}

Node *Edge::getDestination() const
{
    return destination;
}

QString Edge::toGraphML(int indent)
{
    QString xml;
    QTextStream stream(&xml); 

    QString tab = QString("\t").repeated(indent);
    stream << tab;
    stream << "<edge id=\"" << getID() << "\" source=\"" << getSourceID();
    stream << "\" target=\"" << getDestinationID() << "\">\n";
    for(auto data : getData()){
        stream << data->toGraphML(indent + 1);
    }
    stream << tab << "</edge>\n";
    return xml;
}

QString Edge::toString()
{
    QString str;
    QTextStream stream(&str); 
    //Todo get actual edge type
    stream << "[" << getID() << "] Edge " << getSourceID() << " -> " << getDestinationID();
    return str;
}

bool Edge::isConnected(Node* node){
    return source == node || destination == node;
}
bool Edge::isInModel(){
    if(source && destination){
        return source->isInModel() && destination->isInModel();
    }else{
        return false;
    }
}