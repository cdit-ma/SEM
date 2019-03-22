#ifndef WORKLOADREQUEST_H
#define WORKLOADREQUEST_H

#include "eventrequest.h"

class WorkloadRequest : public EventRequest {

public:
    explicit WorkloadRequest();

    void setWorkerInstanceIDs(const QVector<QString>& worker_instance_ids);
    void setWorkerInstancePaths(const QVector<QString>& worker_instance_paths);

    const QVector<QString>& worker_instance_ids() const;
    const QVector<QString>& worker_instance_paths() const;

private:
    QVector<QString> workerInstanceIDs_;
    QVector<QString> workerInstancePaths_;

};

#endif // WORKLOADREQUEST_H
