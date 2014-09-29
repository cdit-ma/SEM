#ifndef InEventPortIDL_H
#define InEventPortIDL_H
#include "eventport.h"
#include "../Workload/ineventport.h"

class InEventPortIDL : public EventPort
{
        Q_OBJECT
public:
    InEventPortIDL(QString name="");
    ~InEventPortIDL();

    void addInEventPortInstance(InEventPort* port);
    void removeInEventPortInstance(InEventPort* port);
    QVector<InEventPort*> getInEventPortInstances();

    // GraphML interface
public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel);
    QString toString();

private:
    QVector<InEventPort*> inEventPorts;

    // GraphMLContainer interface
public:
    void addEdge(Edge *edge);
};

#endif // InEventPortIDL_H
