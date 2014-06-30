#ifndef GRAPH_H
#define GRAPH_H

#include "graphml.h"
class Graph : public GraphML
{
public:
    //Constructor (May have more args)
    Graph(QString name);
    ~Graph();

    const static GraphML::KIND classKind = GraphML::GRAPH;

    // GraphML interface
public:
    bool isAdoptLegal(GraphML *child);
    bool isEdgeLegal(GraphML *attachableObject);
    QString toGraphML(qint32 indentationLevel=0);
    QString toString();

};



#endif // GRAPH_H
