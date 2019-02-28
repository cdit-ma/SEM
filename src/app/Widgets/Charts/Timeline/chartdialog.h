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
    void receivedData();

public slots:
    void themeChanged();

    void toggleTimelineAxisFormat();
    void snapShot();
    void clear();

    void experimentRunSelected(ExperimentRun experimentRun);
    void queryResponseReceived(QList<MEDEA::Event *> events);

    void receivedPortLifecycleResponse(QVector<PortLifecycleEvent*> events);
    void receivedWorkloadResponse(QVector<WorkloadEvent*> events);
    void receivedCPUUtilisationResponse(QVector<CPUUtilisationEvent*> events);
    void receivedMemoryUtilisationResponse(QVector<MemoryUtilisationEvent*> events);

private:
    TimelineChartView* chartView_ = 0;
    ChartInputPopup* inputPopup_ = 0;

    ExperimentRun selectedExperimentRun_;
    bool hasSelectedExperimentRun_ = false;

    TIME_DISPLAY_FORMAT displayFormat_;
};

#endif // CHARTDIALOG_H
