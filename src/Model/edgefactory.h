#ifndef EDGEFACTORY_H
#define EDGEFACTORY_H

#include <QMap>

#include "node.h"
#include "edge.h"


class EdgeFactory
{
public:
    static Edge* createEdge(Node* source, Node* destination, Edge::EDGE_KIND edgeKind);
    static Edge* createEdge(Node* source, Node* destination, QString kind);
    static QString getEdgeKindString(Edge::EDGE_KIND edgeKind);
    static QList<Edge::EDGE_KIND> getEdgeKinds();
    static Edge::EDGE_KIND getEdgeKind(QString kind);


protected:
    EdgeFactory();
    static EdgeFactory* getFactory();
    QMap<Edge::EDGE_KIND, QString> edgeLookup;
    Edge* _createEdge(Node* source, Node* destination, Edge::EDGE_KIND edgeKind);
private:
    static EdgeFactory* factory;
};

#endif // EDGEFACTORY_H
