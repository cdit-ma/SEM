#ifndef EDGE_H
#define EDGE_H
#include <functional>

#include "entity.h"

#include "../nodekinds.h"
#include "../edgekinds.h"


class Node;
class Edge: public Entity{
    Q_OBJECT


protected:
    Edge(EntityFactoryBroker& broker, Node* source, Node* destination, EDGE_KIND kind);
    ~Edge();
public:
    static bool SortByKind(const Edge* a, const Edge* b);
    
    //Get the source graphml object of this Edge
    Node* getSource() const;
    Node* getDestination() const;
    int getSourceID() const;
    int getDestinationID() const;
    bool isConnected(Node* node);

    EDGE_KIND getEdgeKind() const;

    QString toString();
private:
    EDGE_KIND kind;
    
    Node* source = 0;
    Node* destination = 0;
};

#endif // EDGE_H
