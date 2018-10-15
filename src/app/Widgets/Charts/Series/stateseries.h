#ifndef STATESERIES_H
#define STATESERIES_H

#include "dataseries.h"

namespace MEDEA {

class StateSeries : public DataSeries
{
    Q_OBJECT

public:
    explicit StateSeries(ViewItem* item = 0);
    ~StateSeries();

    void addLine(QPointF p1, QPointF p2);
    const QList<QLineF>& getLines();

private:
    QList<QLineF> _lines;

};

}

#endif // STATESERIES_H
