#include "edge.h"
#include "node.h"
#include "data.h"
#include <QDebug>

#include "../entityfactorybroker.h"
#include "../edgekinds.h"

Edge::Edge(EntityFactoryBroker& broker, Node *source, Node *destination, EDGE_KIND edge_kind) : Entity(broker, GRAPHML_KIND::EDGE)
{
    //Set the instance Variables
    this->kind = edge_kind;
    this->source = source;
    this->destination = destination;

    //Attach default data
    broker.AttachData(this, "kind", QVariant::String, ProtectedState::PROTECTED, broker.GetEdgeKindString(edge_kind));

    //Attach the Edge to its source/Destination
    if(source && destination){
        if(source == destination){
            throw std::invalid_argument(source->toString().toStdString() + " Cannot add an edge as both Source and Target.");
        }
        if(!source->canAcceptEdgeKind(edge_kind, EDGE_DIRECTION::SOURCE)){
            throw std::invalid_argument(source->toString().toStdString() + " Cannot add an edge as Source.");
        }

        if(!destination->canAcceptEdgeKind(edge_kind, EDGE_DIRECTION::TARGET)){
            throw std::invalid_argument(source->toString().toStdString() + " Cannot add an edge as Target.");
        }

        if(!source->canAcceptEdge(edge_kind, destination)){
            throw std::invalid_argument(source->toString().toStdString() + " Cannot add edge.");
        }
        source->addEdge(this);
        destination->addEdge(this);
    }else{
        throw std::invalid_argument("Cannot construct an Edge with NULL source or destination");
    }
}

EDGE_KIND Edge::getEdgeKind() const{
    return kind;    
}

Edge::~Edge()
{
    //Deregister first 
    getFactoryBroker().DeregisterEdge(this);
    
    //Remove from the Source/Destination
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

bool Edge::SortByKind(const Edge* a, const Edge* b){
    auto a_ind = static_cast<uint>(a->getEdgeKind());
    auto b_ind = static_cast<uint>(b->getEdgeKind());
    return a_ind > b_ind;
}