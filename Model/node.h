#ifndef NODE_H
#define NODE_H

#include "graphml.h"
#include "graph.h"
#include <QString>

class Node : public GraphML
{
public:
    Node(qint32 nodeType, QString name);
    ~Node();

    const static qint32 classKind = 2;
public:
    //Continue these being Pure Virtual.
    bool isAdoptLegal(GraphML *child)=0;
    bool isEdgeLegal(GraphML *attachableObject)=0;

    virtual QString toGraphML(qint32 indentationLevel=0);
    QString toString();
    Graph * getGraph();

    qint32 getNodeKind() const;

    //Extend the adopt/disown functionality to use the internal graph object in the node.
    void adopt(GraphML *child);
    void disown(GraphML *child);

private:
    Graph* childGraph;
    qint32 nodeKind;

    // GraphML interface
public:

};


#endif // NODE_H
