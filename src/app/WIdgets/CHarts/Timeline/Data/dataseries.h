#ifndef DATASERIES_H
#define DATASERIES_H

#include <QObject>

namespace MEDEA {

class DataSeries : public QObject
{
    Q_OBJECT

public:
    explicit DataSeries(QObject* parent = 0);

    virtual QString getHoveredDataString(qint64 fromTimeMS, qint64 toTimeMS);

};

}

#endif // DATASERIES_H
