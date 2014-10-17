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
    void addInstance(InEventPortInstance* instance);
    void removeInstance(InEventPortInstance* instance);
    QVector<InEventPortInstance*> getInstances();

    void setImpl(InEventPortImpl* impl);
    InEventPortImpl* getImpl();

    QString toString();

    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);
private:
    QVector<InEventPortInstance*> instances;
    InEventPortImpl* impl;

};

#endif // INEVENTPORT_H
