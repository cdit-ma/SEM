#ifndef GRAPH_H
#define GRAPH_H

#include <QVector>
#include "graphmlcontainer.h"

class Graph : public GraphMLContainer
{
    Q_OBJECT
public:
    //Constructor (May have more args)
    Graph(QString name);
    ~Graph();

    QVector<Edge *> getContainedEdges();
    QVector<Edge *> getAllChildrenEdges();

public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    QString toGraphML(qint32 indentationLevel=0);
    QString toString();

    //The unique ID counter;
    static int _Gid;

};



#endif // GRAPH_H
