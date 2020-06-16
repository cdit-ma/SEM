#ifndef MARKERREQUEST_H
#define MARKERREQUEST_H

#include "eventrequest.h"

class MarkerRequest : public EventRequest {

public:
    explicit MarkerRequest() = default;

    void setWorkerInstanceIDs(const QVector<QString>& worker_instance_ids);
    void setWorkerInstancePaths(const QVector<QString>& worker_instance_paths);

    const QVector<QString>& worker_instance_ids() const;
    const QVector<QString>& worker_instance_paths() const;

private:
    QVector<QString> workerInstanceIDs_;
    QVector<QString> workerInstancePaths_;
};

#endif // MARKERREQUEST_H