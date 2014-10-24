#ifndef INEVENTPORTINSTANCE_H
#define INEVENTPORTINSTANCE_H
#include "eventport.h"

class InEventPort;
class InEventPortImpl;

class InEventPortInstance : public Node
{
        Q_OBJECT
public:
    InEventPortInstance(QString name="");
    ~InEventPortInstance();


    // GraphML interface
public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel);
    QString toString();

};

#endif // INEVENTPORTINSTANCE_H
