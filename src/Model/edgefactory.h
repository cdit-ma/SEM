#ifndef EDGEFACTORY_H
#define EDGEFACTORY_H

#include <QHash>
#include <functional>

#include "node.h"
#include "edge.h"



class EdgeFactory
{
private:
    struct EdgeLookupStruct{
        Edge::EDGE_KIND kind;
        QString kind_str;
        std::function<Edge* (Node*, Node*)> constructor;
    };
public:
    static Edge* createEdge(Node* source, Node* destination, Edge::EDGE_KIND edgeKind);
    static Edge* createEdge(Node* source, Node* destination, QString kind);
    static QString getEdgeKindString(Edge::EDGE_KIND edgeKind);
    static QList<Edge::EDGE_KIND> getEdgeKinds();
    static Edge::EDGE_KIND getEdgeKind(QString kind);


protected:
    EdgeFactory();
    ~EdgeFactory();
    static EdgeFactory* getFactory();

    QHash<Edge::EDGE_KIND, EdgeLookupStruct*> edgeLookup;
    QHash<QString, Edge::EDGE_KIND> edgeKindLookup;

    Edge* _createEdge(Node* source, Node* destination, Edge::EDGE_KIND edgeKind);
private:
    void addKind(Edge::EDGE_KIND kind, QString kind_str, std::function<Edge* (Node*, Node*)> constructor);
    static EdgeFactory* factory;
};

#endif // EDGEFACTORY_H
