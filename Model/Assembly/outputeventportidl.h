#ifndef OutputEventPortIDL_H
#define OutputEventPortIDL_H
#include "eventport.h"

class OutputEventPortIDL : public EventPort
{
        Q_OBJECT
public:
    OutputEventPortIDL(QString name="");
    ~OutputEventPortIDL();
    // GraphML interface
public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel);
    QString toString();
};

#endif // OutputEventPortIDL_H
