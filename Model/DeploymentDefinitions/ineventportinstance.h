#ifndef INEVENTPORTINSTANCE_H
#define INEVENTPORTINSTANCE_H
#include "eventport.h"

class InEventPort;
class InEventPortImpl;

class InEventPortInstance : public EventPort
{
        Q_OBJECT
public:
    InEventPortInstance(QString name="");
    ~InEventPortInstance();

    void setDefinition(InEventPort* def);
    InEventPort* getDefinition();

    InEventPortImpl* getImpl();


    // GraphML interface
public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel);
    QString toString();

private:
    InEventPort* def;
    // GraphMLContainer interface
};

#endif // INEVENTPORTINSTANCE_H
