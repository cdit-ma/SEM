#ifndef EDGE_H
#define EDGE_H

#include "entity.h"
#include "edgekinds.h"

class EntityFactory;
class Node;

class Edge: public Entity{
    Q_OBJECT

    //The factory can access protected constructors
    friend class EntityFactory;
protected:
    static void RegisterEdgeKind(EntityFactory* factory, EDGE_KIND kind, QString kind_string, std::function<Edge* (Node*, Node*)> constructor);
    static void RegisterDefaultData(EntityFactory* factory, EDGE_KIND kind, QString key_name, QVariant::Type type, bool is_protected = false, QVariant value = QVariant());
    
    Edge(Node* source, Node* destination, EDGE_KIND kind);
    Edge(EntityFactory* factory, EDGE_KIND kind, QString kind_string);
    ~Edge();
public:
    //Get the source graphml object of this Edge
    Node* getSource() const;
    Node* getDestination() const;
    int getSourceID() const;
    int getDestinationID() const;
    bool isConnected(Node* node);
    bool isInModel();

    EDGE_KIND getEdgeKind() const;

    //Return the graphml representation of this
    QString toGraphML(qint32 indentationLevel=0);
    QString toString();
private:
    EDGE_KIND kind;
    
    Node* source = 0;
    Node* destination = 0;
};

#endif // EDGE_H
