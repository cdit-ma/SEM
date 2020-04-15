#ifndef PORTEVENTREQUEST_H
#define PORTEVENTREQUEST_H

#include "eventrequest.h"

class PortEventRequest : public EventRequest {

public:
    explicit PortEventRequest() = default;

    void setPortIDs(const QVector<QString>& port_ids);
    void setPortPaths(const QVector<QString>& port_paths);

    const QVector<QString>& port_ids() const;
    const QVector<QString>& port_paths() const;

private:
    QVector<QString> portIDs_;
    QVector<QString> portPaths_;
};

#endif // PORTEVENTREQUEST_H