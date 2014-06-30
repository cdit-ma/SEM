#include "graphml.h"

#ifndef GRAPH_H
#define GRAPH_H

class Graph : public GraphML
{
public:
    //Constructor (May have more args)
    Graph(QString name);
    ~Graph();

    const static qint32 classKind = 1;
    // GraphML interface
public:
    bool isAdoptLegal(GraphML *child);
    bool isEdgeLegal(GraphML *attachableObject);
    QString toGraphML(qint32 indentationLevel=0);
    QString toString();

};



#endif // GRAPH_H
