#ifndef CHARTINPUTPOPUP_H
#define CHARTINPUTPOPUP_H

#include "hoverpopup.h"
#include "../Data/Events/protoMessageStructs.h"

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

signals:
    void requestExperimentRuns(QString experimentName = "");
    void requestExperimentState(quint32 experimentRunID);
    void requestEvents(QString node, QString component, QString worker);

    void setChartTitle(QString title);

public slots:
    void themeChanged();

    void setPopupVisible(bool visible);
    void populateExperimentRuns(QList<ExperimentRun> runs);

    void receivedExperimentState(QStringList nodeHostnames, QStringList componentNames, QStringList workerNames);
    void filterMenuTriggered(QAction* action);

    void accept();
    void reject();

private:
    void populateGroupBox(FILTER_KEY filter);
    void clearGroupBox(FILTER_KEY filter);
    void hideGroupBoxes();
    void recenterPopup();
    void resizePopup();

    QString& getSelectedFilter(FILTER_KEY filter);
    QStringList& getFilterList(FILTER_KEY filter);
    QGroupBox* getFilterGroupBox(FILTER_KEY filter);
    QGroupBox* constructFilterWidgets(FILTER_KEY filter, QString filterName, bool addToFilterMenu = true, bool scrollable = false);

    QLineEdit* experimentNameLineEdit_;

    QGroupBox* experimentNameGroupBox_ = 0;
    QGroupBox* experimentRunsGroupBox_ = 0;
    QGroupBox* nodesGroupBox_ = 0;
    QGroupBox* componentsGroupBox_ = 0;
    QGroupBox* workersGroupBox_ = 0;

    QHash<FILTER_KEY, QLayout*> groupBoxLayouts;

    QToolBar* toolbar_;
    QAction* okAction_;
    QAction* cancelAction_;
    QAction* filterAction_;
    QMenu* filterMenu_;
    QWidget* scrollWidget;

    QPointF originalCenterPos_;

    qint32 selectedExperimentRunID_;
    QString experimentName_;
    QString selectedNode_;
    QString selectedComponent_;
    QString selectedWorker_;

    QStringList nodes_;
    QStringList components_;
    QStringList workers_;
};

#endif // CHARTINPUTPOPUP_H
