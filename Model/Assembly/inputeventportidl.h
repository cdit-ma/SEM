#ifndef InputEventPortIDL_H
#define InputEventPortIDL_H
#include "eventport.h"

class InputEventPortIDL : public EventPort
{
        Q_OBJECT
public:
    InputEventPortIDL(QString name="");
    ~InputEventPortIDL();

    // GraphML interface
public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel);
    QString toString();
};

#endif // InputEventPortIDL_H
