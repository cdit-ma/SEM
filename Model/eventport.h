#ifndef EVENTPORT_H
#define EVENTPORT_H

#include "node.h"
#include <QString>

class EventPort : public Node
{
public:
    EventPort(qint32 portType, QString name);
    ~EventPort();


    const static qint32 nodeKind = 4;
    // GraphML interface
public:
    bool isAdoptLegal(GraphML *child)=0;
    bool isEdgeLegal(GraphML *attachableObject)=0;
    QString toGraphML(qint32 indentationLevel=0);
    QString toString();

private:
    qint32 portKind;
};

#endif // EVENTPORT_H
