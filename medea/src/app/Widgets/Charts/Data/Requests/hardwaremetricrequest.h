#ifndef HARDWAREMETRICREQUEST_H
#define HARDWAREMETRICREQUEST_H

#include "request.h"

class HardwareMetricRequest : public Request {

public:
    explicit HardwareMetricRequest() = default;

    void setNodeIDs(const QVector<QString>& node_ids);
    void setNodeHostnames(const QVector<QString>& node_hostnames);

    const QVector<QString>& node_ids() const;
    const QVector<QString>& node_hostnames() const;

private:
    QVector<QString> nodeIDs_;
    QVector<QString> nodeHostnames_;
};

#endif // HARDWAREMETRICREQUEST_H