#include "eventport.h"

#ifndef INPUTEVENTPORT_H
#define INPUTEVENTPORT_H

class InputEventPort : public EventPort
{
public:
    InputEventPort(QString name);
    ~InputEventPort();

    const static qint32 portKind = 1;
    // GraphML interface
public:
    bool isAdoptLegal(GraphML *child);
    bool isEdgeLegal(GraphML *attachableObject);
    QString toGraphML(qint32 indentationLevel);
    QString toString();
};

#endif // INPUTEVENTPORT_H
