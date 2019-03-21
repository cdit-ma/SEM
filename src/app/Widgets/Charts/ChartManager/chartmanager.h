#ifndef CHARTMANAGER_H
#define CHARTMANAGER_H

#include <QObject>

#include "../../../Controllers/ViewController/viewcontroller.h"
#include "../Timeline/chartdialog.h"
#include "../Timeline/chartinputpopup.h"
#include "../Timeline/Chart/timelinechartview.h"

class RequestBuilder;
class ChartManager : public QObject
{
    Q_OBJECT
    friend class ViewController;

public:
    static ChartManager* manager();

    ChartDialog* getChartDialog();

signals:
    void showChartsPanel();

public slots:
    void acquireExperimentRun();

    void filterRequestsBySelectedEntities(const QVector<ViewItem*>& selectedItems, const QList<TIMELINE_DATA_KIND>& selectedDataKinds);
    void experimentRunSelected(const ExperimentRun& experimentRun);

protected:
    static void constructSingleton(ViewController* vc);

private:
    ChartManager(ViewController* vc);
    ~ChartManager();

    void experimentRunStateReceived(ExperimentRun experimentRun, ExperimentState experimentState);
    void requestEventsForExperimentRun(const ExperimentRun &experimentRun);

    void requestExperimentRuns(const QString& experimentName);
    void requestExperimentState(const quint32 experimentRunID);

    void requestEvents(RequestBuilder* builder, const ExperimentRun &experimentRun);
    void requestPortLifecycleEvents(const PortLifecycleRequest& request, const ExperimentRun& experimentRun);
    void requestWorkloadEvents(const WorkloadRequest& request, const ExperimentRun& experimentRun);
    void requestCPUUtilisationEvents(const CPUUtilisationRequest& request, const ExperimentRun& experimentRun);
    void requestMemoryUtilisationEvents(const MemoryUtilisationRequest& request, const ExperimentRun& experimentRun);
    void requestMarkerEvents(const MarkerRequest& request, const ExperimentRun& experimentRun);

    void toastNotification(const QString& description, const QString& iconName, Notification::Severity severity = Notification::Severity::INFO);

    QString getItemLabel(const ViewItem* item);

    ChartDialog* chartDialog_ = 0;
    ChartInputPopup* chartPopup_ = 0;
    TimelineChartView* chartView_ = 0;
    ViewController* viewController_ = 0;

    QVector<ViewItem*> selectedViewItems_;
    QList<TIMELINE_DATA_KIND> selectedDataKinds_;

    static ChartManager* manager_;
};

#endif // CHARTMANAGER_H
