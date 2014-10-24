#ifndef INEVENTPORT_H
#define INEVENTPORT_H
#include "../node.h"

class InEventPortInstance;
class InEventPortImpl;

class InEventPort : public Node
{
public:
    InEventPort(QString name="");
    ~InEventPort();

    QString toString();

    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);
private:
    QVector<InEventPortInstance*> instances;
    InEventPortImpl* impl;

};

#endif // INEVENTPORT_H
