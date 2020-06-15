#ifndef PORTLIFECYCLEREQUEST_H
#define PORTLIFECYCLEREQUEST_H

#include "eventrequest.h"

class PortLifecycleRequest : public EventRequest {

public:
    explicit PortLifecycleRequest() = default;

    void setPortIDs(const QVector<QString>& port_ids);
    void setPortPaths(const QVector<QString>& port_paths);

    const QVector<QString>& port_ids() const;
    const QVector<QString>& port_paths() const;

private:
    QVector<QString> portIDs_;
    QVector<QString> portPaths_;
};

#endif // PORTLIFECYCLEREQUEST_H