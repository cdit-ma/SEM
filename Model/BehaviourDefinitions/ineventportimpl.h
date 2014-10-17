#ifndef INEVENTPORTIMPL_H
#define INEVENTPORTIMPL_H
#include "../DeploymentDefinitions/eventport.h"

class InEventPort;

class InEventPortImpl : public Node
{
    Q_OBJECT
public:
    InEventPortImpl(QString name="");
    ~InEventPortImpl();

    void setDefinition(InEventPort *parent);
    InEventPort* getDefinition();
public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel=0);
    QString toString();

private:
    InEventPort* def;
};


#endif // INEVENTPORT_H
