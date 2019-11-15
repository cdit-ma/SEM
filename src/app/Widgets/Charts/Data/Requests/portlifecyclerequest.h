#ifndef PORTLIFECYCLEREQUEST_H
#define PORTLIFECYCLEREQUEST_H

#include "eventrequest.h"

class PortLifecycleRequest : public EventRequest {

public:
    explicit PortLifecycleRequest();

    void setPortIDs(const QVector<QString>& port_ids);
    void setPortPaths(const QVector<QString>& port_paths);

    const QVector<QString>& port_ids() const;
    const QVector<QString>& port_paths() const;

private:
    QVector<QString> portIDs_;
    QVector<QString> portPaths_;
};

//Q_DECLARE_METATYPE(PortLifecycleRequest);

#endif // PORTLIFECYCLEREQUEST_H
