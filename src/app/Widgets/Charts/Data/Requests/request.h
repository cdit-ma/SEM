#ifndef REQUEST_H
#define REQUEST_H

//#include "../Events/protomessagestructs.h"
#include "../Events/event.h"

class Request {

protected:
    explicit Request(MEDEA::ChartDataKind kind = MEDEA::ChartDataKind::DATA);

public:
    void setExperimentID(const quint32 experiment_run_id);
    void setTimeInterval(const QVector<qint64>& time_interval);

    quint32 experiment_run_id() const;
    MEDEA::ChartDataKind getKind() const;

    const QVector<qint64>& time_interval() const;

private:
    quint32 experimentRunID_;
    QVector<qint64> timeInterval_;

    MEDEA::ChartDataKind kind_;

};

#endif // REQUEST_H
