#ifndef INPUTEVENTPORT_H
#define INPUTEVENTPORT_H
#include "eventport.h"

class InputEventPort : public EventPort
{
public:
    InputEventPort(QString name);
    ~InputEventPort();

    // GraphML interface
public:
    bool isAdoptLegal(GraphMLContainer *child);
    bool isEdgeLegal(GraphMLContainer *attachableObject);
    //QString toGraphML(qint32 indentationLevel);
    QString toString();
};

#endif // INPUTEVENTPORT_H
