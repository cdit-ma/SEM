#ifndef NODE_H
#define NODE_H

#include "graphmlcontainer.h"
#include "graph.h"
#include <QString>

class Node : public GraphMLContainer
{
        Q_OBJECT
public:
    Node(QString name);
    ~Node();

public:
    //Continue these being Pure Virtual.
    bool isAdoptLegal(GraphMLContainer *child)=0;
    bool isEdgeLegal(GraphMLContainer *attachableObject)=0;

    QString toGraphML(qint32 indentationLevel=0);
    QString toString();
    Graph* getGraph();


    //Extend the adopt/disown functionality to use the internal graph object in the node.
    void adopt(GraphMLContainer* child);
    void disown(GraphMLContainer* child);
private:
    Graph* childGraph;
    static int _Nid;
};


#endif // NODE_H
