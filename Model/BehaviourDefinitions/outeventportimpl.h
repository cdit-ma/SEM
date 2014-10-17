#ifndef OUTEVENTPORTIMPL_H
#define OUTEVENTPORTIMPL_H
#include "../DeploymentDefinitions/eventport.h"

class OutEventPort;

class OutEventPortImpl : public Node
{
    Q_OBJECT
public:
    OutEventPortImpl(QString name="");
    ~OutEventPortImpl();

    void setDefinition(OutEventPort *parent);
    OutEventPort* getDefinition();
public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel=0);
    QString toString();

private:
    OutEventPort* def;
};

#endif
