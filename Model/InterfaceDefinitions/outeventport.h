#ifndef OUTEVENTPORT_H
#define OUTEVENTPORT_H
#include "../node.h"


class OutEventPortInstance;
class OutEventPortImpl;

class OutEventPort : public Node
{
public:
    OutEventPort(QString name="");
    ~OutEventPort();

    void addInstance(OutEventPortInstance* instance);
    void removeInstance(OutEventPortInstance* instance);
    QVector<OutEventPortInstance*> getInstances();

    void setImpl(OutEventPortImpl* impl);
    OutEventPortImpl* getImpl();

    QString toString();

    bool isEdgeLegal(GraphMLContainer *attachableObject);
    bool isAdoptLegal(GraphMLContainer *child);
private:
    QVector<OutEventPortInstance*> instances;
    OutEventPortImpl* impl;
};

#endif // OUTEVENTPORT_H
