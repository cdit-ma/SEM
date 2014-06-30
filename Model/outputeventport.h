#ifndef OUTPUTEVENTPORT_H
#define OUTPUTEVENTPORT_H
#include "eventport.h"

class OutputEventPort : public EventPort
{
public:
    OutputEventPort(QString name);
    ~OutputEventPort();
    // GraphML interface
public:
    bool isAdoptLegal(GraphML *child);
    bool isEdgeLegal(GraphML *attachableObject);
    QString toGraphML(qint32 indentationLevel);
    QString toString();
};

#endif // OUTPUTEVENTPORT_H
