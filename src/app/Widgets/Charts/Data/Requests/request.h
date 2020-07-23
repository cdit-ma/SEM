#ifndef REQUEST_H
#define REQUEST_H

#include "../Events/event.h"

class Request {

protected:
    explicit Request() = default;

public:
    void setExperimentRunID(quint32 experiment_run_id);
    void setExperimentName(const QString& experiment_name);
    void setTimeInterval(const QVector<qint64>& time_interval);

    quint32 experiment_run_id() const;
    QString experiment_name() const;
    const QVector<qint64>& time_interval() const;

private:
    quint32 experimentRunID_ = 0;
    QString experimentName_;
    QVector<qint64> timeInterval_;
};

#endif // REQUEST_H