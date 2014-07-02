#ifndef GRAPH_H
#define GRAPH_H

#include "graphmlcontainer.h"
class Graph : public GraphMLContainer
{
public:
    //Constructor (May have more args)
    Graph(QString name);
    ~Graph();


public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    QString toGraphML(qint32 indentationLevel=0);
    QString toString();

    //The unique ID counter;
    static int _Gid;

};



#endif // GRAPH_H
