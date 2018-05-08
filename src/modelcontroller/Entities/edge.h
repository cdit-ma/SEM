#ifndef EDGE_H
#define EDGE_H
#include <functional>

#include "entity.h"

#include "../nodekinds.h"
#include "../edgekinds.h"

class EntityFactory;
class Node;

class Edge: public Entity{
    Q_OBJECT

    //The factory can access protected constructors
    friend class EntityFactory;
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory, const EDGE_KIND& edge_kind, const QString& kind_string, std::function<Edge* (EntityFactory&, Node*, Node*)> constructor);
    Edge(EntityFactory& factory, Node* source, Node* destination, EDGE_KIND kind);
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

    //Return the graphml representation of this
    QString toGraphML(int indentationLevel = 0, bool function_only = false);
    QString toString();
private:
    EDGE_KIND kind;
    
    Node* source = 0;
    Node* destination = 0;
};

#endif // EDGE_H
