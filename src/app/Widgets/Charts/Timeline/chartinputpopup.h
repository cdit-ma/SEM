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
    explicit ChartInputPopup(QWidget* parent = 0);

signals:
    void requestExperimentRuns(QString experimentName = "");
    void requestExperimentState(quint32 experimentRunID);
    void requestEvents(QString node, QString component, QString worker);

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
    void recenterPopup();
    void hideFilters();

    QString& getSelectedFilter(FILTER_KEY filter);
    QStringList& getFilterList(FILTER_KEY filter);
    QGroupBox* getFilterGroupBox(FILTER_KEY filter);
    QGroupBox* constructFilterWidgets(FILTER_KEY filter, QString filterName);

    QGroupBox* experimentNameGroupBox_;
    QGroupBox* experimentRunsGroupBox_;
    QGroupBox* nodesGroupBox_;
    QGroupBox* componentsGroupBox_;
    QGroupBox* workersGroupBox_;

    QLineEdit* experimentNameLineEdit_;

    QToolBar* toolbar_;
    QAction* okAction_;
    QAction* cancelAction_;
    QAction* filterAction_;
    QMenu* filterMenu_;

    QPointF originalCenterPos_;

    qint32 selectedExperimentRunID_;
    QString selectedNode_;
    QString selectedComponent_;
    QString selectedWorker_;

    QStringList nodes_;
    QStringList components_;
    QStringList workers_;
};

#endif // CHARTINPUTPOPUP_H
