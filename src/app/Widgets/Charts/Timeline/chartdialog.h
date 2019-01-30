#ifndef CHARTDIALOG_H
#define CHARTDIALOG_H

#include <QFrame>

#include "../../../Controllers/ViewController/viewcontroller.h"
#include "Chart/timelinechartview.h"
#include "chartinputpopup.h"

class ChartDialog : public QFrame
{
    Q_OBJECT

public:
    explicit ChartDialog(ViewController* vc, QWidget* parent = 0);

signals:
    void receivedNewData();

public slots:
    void themeChanged();

    void snapShot();
    void clear();

private:
    TimelineChartView* chartView_ = 0;
    ChartInputPopup* inputPopup_ = 0;

};

#endif // CHARTDIALOG_H
