#ifndef INEVENTPORT_H
#define INEVENTPORT_H
#include "../DeploymentDefinitions/eventport.h"

class InEventPortIDL;

class InEventPort : public Node
{
    Q_OBJECT
public:
    InEventPort(QString name="");
    ~InEventPort();
    void setInEventPortIDL(InEventPortIDL *parent);
    InEventPortIDL* getInEventPortIDL();
public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel=0);
    QString toString();

private:
    InEventPortIDL* inEventPortIDL;
};


#endif // INEVENTPORT_H
