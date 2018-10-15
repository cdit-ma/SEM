#ifndef LINESERIES_H
#define LINESERIES_H

#include "dataseries.h"

namespace MEDEA {

class LineSeries : public DataSeries
{
    Q_OBJECT

public:
    explicit LineSeries(ViewItem* item = 0);
    ~LineSeries();

    void addLine(QPointF p1, QPointF p2);
    void addLine(QLineF line);
    void addLines(QList<QLineF> lines);

    QList<QLineF> getLines();

signals:
    void linesAdded(QList<QLineF> lines);

private:
    QList<QLineF> _lines;

};

}

#endif // LINESERIES_H
