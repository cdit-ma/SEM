#ifndef BASESERIES_H
#define BASESERIES_H

#include <QObject>


class BaseSeries : public QObject
{
    Q_OBJECT

protected:
    explicit BaseSeries(QObject* parent = 0);

public:
    virtual QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS);

};



#endif // BASESERIES_H
