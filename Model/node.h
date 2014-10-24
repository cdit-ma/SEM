#ifndef NODE_H
#define NODE_H

#include "graphmlcontainer.h"
#include "graph.h"

#include <QStandardItem>
#include <QString>

class Node : public GraphMLContainer
{
        Q_OBJECT
public:
    enum NODE_TYPE {NT_NODE, NT_DEFINITION, NT_INSTANCE, NT_IMPL};
    Node(QString name, NODE_TYPE type = NT_NODE);
    ~Node();

signals:
    //Used to construct and tear down GUI elements.
    void constructGUI(GraphMLContainer*);
    void destructGUI(GraphMLContainer*, QString ID);
public:

    //Continue these being Pure Virtual.
    bool isAdoptLegal(GraphMLContainer *child)=0;
    bool isEdgeLegal(GraphMLContainer *attachableObject)=0;

    void addAspect(QString aspect);
    void removeAspect(QString aspect);
    bool inAspect(QString aspect);

    Node* getParentNode();
    QString toGraphML(qint32 indentationLevel=0);
    QString toString();


    bool isDefinition();
    bool isInstance();
    bool isImpl();

    void setDefinition(Node *def);
    void unsetDefinition();

    void addInstance(Node* inst);
    void removeInstance(Node* inst);

    void setImplementation(Node* impl);
    void unsetImplementation();



private:
    NODE_TYPE nodeType;

    //Used Sparingly
    QVector<GraphMLContainer *> instances;
    GraphMLContainer * definition;
    GraphMLContainer * implementation;

    //Graph* childGraph;
    QVector<QString> containedAspects;
    static int _Nid;

    // QStandardItem interface
};


#endif // NODE_H
