#ifndef InEventPortIDL_H
#define InEventPortIDL_H
#include "eventport.h"

class InEventPortIDL : public EventPort
{
        Q_OBJECT
public:
    InEventPortIDL(QString name="");
    ~InEventPortIDL();

    // GraphML interface
public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel);
    QString toString();
};

#endif // InEventPortIDL_H
