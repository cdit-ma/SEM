#ifndef MODEL_H
#define MODEL_H

#include "graph.h"
#include "edge.h"
#include "node.h"

#include <QString>
#include <QVector>



class Model
{
public:
    Model();

    //Imports
    bool importGraphML(Graph *parent=0);
    QString exportGraphML();
private:
    Graph * parentGraph;
    QVector<Graph*> graphs;
    QVector<Edge *> edges;
    QVector<Node *> nodes;
};

#endif // MODEL_H
