#ifndef EVENTPORT_H
#define EVENTPORT_H

#include "node.h"
#include <QString>

class EventPort : public Node
{
public:
    EventPort(QString name);
    ~EventPort();
    // GraphML interface
public:
    bool isAdoptLegal(GraphMLContainer *child)=0;
    bool isEdgeLegal(GraphMLContainer *attachableObject)=0;
    QString toGraphML(qint32 indentationLevel=0)=0;
    QString toString();
private:
    QString type;
};

#endif // EVENTPORT_H
