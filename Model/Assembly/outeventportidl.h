#ifndef OutEventPortIDL_H
#define OutEventPortIDL_H
#include "eventport.h"

class OutEventPortIDL : public EventPort
{
        Q_OBJECT
public:
    OutEventPortIDL(QString name="");
    ~OutEventPortIDL();
    // GraphML interface
public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel);
    QString toString();
};

#endif // OutEventPortIDL_H
