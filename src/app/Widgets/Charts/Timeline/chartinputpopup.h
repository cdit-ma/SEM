#ifndef CHARTINPUTPOPUP_H
#define CHARTINPUTPOPUP_H

#include "Chart/hoverpopup.h"
#include "../Data/Events/event.h"
#include "../Data/Events/protoMessageStructs.h"
#include "../../../Controllers/ViewController/nodeviewitem.h"
#include "../../../Controllers/ViewController/viewcontroller.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QToolBar>
#include <QAction>
#include <QMenu>

class ChartInputPopup : public HoverPopup
{
    Q_OBJECT

public:
    enum FILTER_KEY{RUNS_FILTER, NODE_FILTER, COMPONENT_FILTER, WORKER_FILTER};

    static QList<FILTER_KEY> getFilterKeys() {
        return {RUNS_FILTER, NODE_FILTER, COMPONENT_FILTER, WORKER_FILTER};
    }

    explicit ChartInputPopup(QWidget* parent = 0);

    void setViewController(ViewController* controller);
    void enableFilters();

signals:
    void setChartTitle(QString title);
    void setExperimentRunID(quint32 experimentRnID);
    void setEventKinds(QList<TIMELINE_DATA_KIND> kinds);

    void requestExperimentRuns(QString experimentName = "");
    void requestExperimentState(quint32 experimentRunID);
    void requestAllEvents();

    void requestPortLifecycleEvents(PortLifecycleRequest request);
    void requestWorkloadEvents(WorkloadRequest request);
    void requestCPUUtilisationEvents(CPUUtilisationRequest request);
    void requestMemoryUtilisationEvents(MemoryUtilisationRequest request);

public slots:
    void themeChanged();

    void setPopupVisible(bool visible);
    void populateExperimentRuns(QList<ExperimentRun> runs);

    void receivedExperimentState(QStringList nodeHostnames, QStringList componentNames, QStringList workerNames);
    void receivedSelectedViewItems(QVector<ViewItem*> selectedItems, QList<TIMELINE_DATA_KIND>& dataKinds);

    void filterMenuTriggered(QAction* action);

    void accept();
    void reject();

private:
    QString getItemLabel(ViewItem* item);

    void populateGroupBox(FILTER_KEY filter);
    void clearGroupBox(FILTER_KEY filter);
    void hideGroupBoxes();

    void recenterPopup();
    void resizePopup();

    void resetFilters();
    void setupFilterWidgets();

    QString& getSelectedFilter(FILTER_KEY filter);
    QStringList& getFilterList(FILTER_KEY filter);
    QGroupBox* getFilterGroupBox(FILTER_KEY filter);

    QGroupBox* constructFilterWidgets(FILTER_KEY filter, QString filterName);
    QVBoxLayout* constructVBoxLayout(QWidget* widget, int spacing = 0, int margin = 0);

    QLineEdit* experimentNameLineEdit_ = 0;
    QWidget* experimentRunsScrollWidget_ = 0;

    QGroupBox* experimentNameGroupBox_ = 0;
    QGroupBox* experimentRunsGroupBox_ = 0;
    QGroupBox* nodesGroupBox_ = 0;
    QGroupBox* componentsGroupBox_ = 0;
    QGroupBox* workersGroupBox_ = 0;

    QHash<FILTER_KEY, QLayout*> groupBoxLayouts;

    QToolBar* toolbar_ = 0;
    QAction* okAction_ = 0;
    QAction* cancelAction_ = 0;
    QAction* filterAction_ = 0;
    QMenu* filterMenu_ = 0;

    bool filtersEnabled_ = false;
    bool hasSelection_ = false;

    QPointF originalCenterPos_;

    qint32 selectedExperimentRunID_;
    QString experimentName_;
    QString selectedNode_;
    QString selectedComponent_;
    QString selectedWorker_;

    QStringList selectedNodes_;
    QStringList selectedComponents_;
    QStringList selectedWorkers_;
    QStringList selectedComponentInstancePaths_;
    QStringList selectedPortInstancePaths_;
    QStringList selectedWorkloadInstancePaths_;

    QStringList nodes_;
    QStringList components_;
    QStringList workers_;

    QList<TIMELINE_DATA_KIND> eventKinds_;
    QStringList compNames_;
    QStringList compInstPaths_;
    QStringList portPaths_;
    QStringList workerPaths_;
    QStringList nodeIDs_;

    ViewController* viewController_ = 0;
};

#endif // CHARTINPUTPOPUP_H
