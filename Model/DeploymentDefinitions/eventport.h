#ifndef EVENTPORT_H
#define EVENTPORT_H

#include "../node.h"
#include <QString>

class EventPort : public Node
{
        Q_OBJECT
public:
    EventPort(QString name);
    ~EventPort();
    // GraphML interface
public:
    //QString toGraphML(qint32 indentationLevel=0);
    QString toString();
private:
    QString type;
};

#endif // EVENTPORT_H
