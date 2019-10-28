#ifndef CHARTMANAGER_H
#define CHARTMANAGER_H

#include <QObject>

#include "../../../Controllers/ViewController/viewcontroller.h"
#include "../Timeline/chartdialog.h"
#include "../Timeline/chartinputpopup.h"
#include "../Timeline/Chart/timelinechartview.h"
#include "../../Dataflow/dataflowdialog.h"

class RequestBuilder;
class ChartManager : public QObject
{
    Q_OBJECT
    friend class ViewController;

public:
    static ChartManager* manager();

    ChartDialog& getChartDialog();
    DataflowDialog& getDataflowDialog();

signals:
    void showChartsPanel();
    void showDataflowPanel();

public slots:
    void displayChartPopup();
    void filterRequestsBySelectedEntities(const QVector<ViewItem*>& selectedItems, const QList<MEDEA::ChartDataKind>& selectedDataKinds);

private slots:
    void experimentRunSelected(const AggServerResponse::ExperimentRun& experimentRun);

protected:
    static void constructSingleton(ViewController* vc);

private:
    ChartManager(const ViewController &vc);
    AggregationProxy& aggregationProxy();
    TimelineChartView& timelineChartView();

    void experimentRunStateReceived(AggServerResponse::ExperimentState experimentState);
    void setupRequestsForExperimentRun(const quint32 experimentRunID);

    void requestExperimentRuns(const QString& experimentName);
    void requestExperimentState(const quint32 experimentRunID);

    void requestEvents(const RequestBuilder &builder);
    void requestPortLifecycleEvents(const PortLifecycleRequest& request, const AggServerResponse::ExperimentRun& experimentRun);
    void requestWorkloadEvents(const WorkloadRequest& request, const AggServerResponse::ExperimentRun& experimentRun);
    void requestCPUUtilisationEvents(const CPUUtilisationRequest& request, const AggServerResponse::ExperimentRun& experimentRun);
    void requestMemoryUtilisationEvents(const MemoryUtilisationRequest& request, const AggServerResponse::ExperimentRun& experimentRun);
    void requestMarkerEvents(const MarkerRequest& request, const AggServerResponse::ExperimentRun& experimentRun);
    void requestPortEvents(const PortEventRequest& request, const AggServerResponse::ExperimentRun& experimentRun);

    static void toastNotification(const QString& description, const QString& iconName, Notification::Severity severity = Notification::Severity::INFO);

    QString getItemLabel(const ViewItem* item);

    DataflowDialog* dataflowDialog_ = nullptr;
    ChartDialog* chartDialog_ = nullptr;
    ChartInputPopup chartPopup_;

    QVector<ViewItem*> selectedViewItems_;
    QList<MEDEA::ChartDataKind> selectedDataKinds_;
    AggServerResponse::ExperimentRun selectedExperimentRun_;

    const ViewController& viewController_;
    static ChartManager* manager_;
};

#endif // CHARTMANAGER_H
