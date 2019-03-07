#ifndef CHARTMANAGER_H
#define CHARTMANAGER_H

#include <QObject>

#include "../../../Controllers/ViewController/viewcontroller.h"
#include "../Timeline/chartdialog.h"
#include "../Timeline/chartinputpopup.h"
#include "../Timeline/Chart/timelinechartview.h"

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
    void showChartsPopup();

    void filterRequestsBySelection(const QVector<ViewItem*>& selectedItems, const QList<TIMELINE_DATA_KIND>& selectedDataKinds);

    void requestEventsForExperimentRun(const ExperimentRun& experimentRun);

protected:
    static void constructSingleton(ViewController* vc);

private:
    ChartManager(ViewController* vc);
    ~ChartManager();

    void requestExperimentRuns(const QString& experimentName);
    void requestExperimentState(const quint32 experiment_run_id);

    void requestPortLifecycleEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& component_instance_ids,
        const QVector<QString>& port_ids);

    void requestWorkloadEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& component_instance_ids,
        const QVector<QString>& worker_ids);

    void requestCPUUtilisationEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& graphml_ids);

    void requestMemoryUtilisationEvents(
        const quint32 experiment_run_id,
        const QVector<qint64>& time_intervals,
        const QVector<QString>& graphml_ids);

    void toastNotification(const QString& description, const QString& iconName, Notification::Severity severity = Notification::Severity::INFO);
    void resetFilters();

    QString getItemLabel(ViewItem* item);

    ExperimentRun selectedExperimentRun_;
    bool hasEntitySelection_ = false;

    QList<TIMELINE_DATA_KIND> eventKinds_;
    QVector<QString> compNames_;
    QVector<QString> compInstPaths_;
    QVector<QString> compInstIDs_;
    QVector<QString> portPaths_;
    QVector<QString> portIDs_;
    QVector<QString> workerInstPaths_;
    QVector<QString> workerInstIDs_;
    QVector<QString> nodeHostnames_;
    QVector<QString> nodeIDs_;

    ChartDialog* chartDialog_ = 0;
    ChartInputPopup* chartPopup_ = 0;
    TimelineChartView* chartView_ = 0;
    ViewController* viewController_ = 0;

    static ChartManager* manager_;
};

#endif // CHARTMANAGER_H
