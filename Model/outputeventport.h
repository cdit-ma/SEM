#include "eventport.h"

#ifndef OUTPUTEVENTPORT_H
#define OUTPUTEVENTPORT_H

class OutputEventPort : public EventPort
{
public:
    OutputEventPort(QString name);
    ~OutputEventPort();

    const static qint32 portKind = 2;
    // GraphML interface
public:
    bool isAdoptLegal(GraphML *child);
    bool isEdgeLegal(GraphML *attachableObject);
    QString toGraphML(qint32 indentationLevel);
    QString toString();
};

#endif // OUTPUTEVENTPORT_H
