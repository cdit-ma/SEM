#ifndef REQUEST_H
#define REQUEST_H

#include "../Events/event.h"

class Request {

protected:
    explicit Request() = default;

public:
    void setExperimentRunID(quint32 experiment_run_id);
    void setTimeInterval(const QVector<qint64>& time_interval);

    quint32 experiment_run_id() const;
    const QVector<qint64>& time_interval() const;

private:
    quint32 experimentRunID_ = 0;
    QVector<qint64> timeInterval_;
};

#endif // REQUEST_H