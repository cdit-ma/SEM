#ifndef UTILISATIONREQUEST_H
#define UTILISATIONREQUEST_H

#include "request.h"

class UtilisationRequest : public Request {

protected:
    explicit UtilisationRequest() = default;

public:
    void setNodeIDs(const QVector<QString>& node_ids);
    void setNodeHostnames(const QVector<QString>& node_hostnames);

    const QVector<QString>& node_ids() const;
    const QVector<QString>& node_hostnames() const;

private:
    QVector<QString> nodeIDs_;
    QVector<QString> nodeHostnames_;
};

#endif // UTILISATIONREQUEST_H