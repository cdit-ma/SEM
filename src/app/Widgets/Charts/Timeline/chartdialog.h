#ifndef CHARTDIALOG_H
#define CHARTDIALOG_H

#include <QFrame>

#include "../../../Controllers/ViewController/viewcontroller.h"
#include "Chart/timelinechartview.h"

class ChartDialog : public QFrame
{
    Q_OBJECT

public:
    explicit ChartDialog(QWidget* parent = nullptr);

    TimelineChartView& getChartView();

public slots:
    void themeChanged();

    void toggleTimelineAxisFormat();
    void snapShot();
    void clear();

private:
    void setupChartView();

    TimelineChartView chartView_;
    TIME_DISPLAY_FORMAT displayFormat_;
};

#endif // CHARTDIALOG_H
