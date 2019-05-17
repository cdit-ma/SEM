#ifndef CHARTINPUTPOPUP_H
#define CHARTINPUTPOPUP_H

#include "Chart/hoverpopup.h"
#include "../Data/Events/event.h"
#include "../Data/Events/protomessagestructs.h"
#include "../../../Controllers/ViewController/nodeviewitem.h"
#include "../../../Controllers/ViewController/viewcontroller.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QCompleter>
#include <QStringListModel>

class ChartInputPopup : public HoverPopup
{
    Q_OBJECT

public:
    enum FILTER_KEY{RUNS_FILTER, NODE_FILTER, COMPONENT_FILTER, WORKER_FILTER};

    static QList<FILTER_KEY> getFilterKeys() {
        return {RUNS_FILTER, NODE_FILTER, COMPONENT_FILTER, WORKER_FILTER};
    }

    explicit ChartInputPopup(QWidget* parent = 0);

    void enableFilters();

    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void setChartTitle(QString title);
    void selectedExperimentRun(const AggServerResponse::ExperimentRun& experimentRun);

public slots:
    void themeChanged();

    void setPopupVisible(bool visible);
    void setExperimentRuns(const QList<AggServerResponse::ExperimentRun>& runs);

    void filterMenuTriggered(QAction* action);

    void accept();
    void reject();

private slots:
    void experimentNameChanged(const QString& experimentName);
    void experimentNameActivated(const QString& experimentName);
    void experimentRunSelected(const AggServerResponse::ExperimentRun& experimentRun);

private:
    void populateExperimentRuns(const QList<AggServerResponse::ExperimentRun>& runs);

    void populateGroupBox(FILTER_KEY filter);
    void clearGroupBox(FILTER_KEY filter);
    void resetGroupBox(FILTER_KEY filter);
    void setGroupBoxVisible(FILTER_KEY filter, bool visible);

    void recenterPopup();
    void resizePopup();
    void resetPopup();

    void setupLayout();
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

    QPointF originalCenterPos_;
    bool filtersEnabled_ = false;

    AggServerResponse::ExperimentRun selectedExperimentRun_;
    qint32 selectedExperimentRunID_;

    QString selectedNode_;
    QString selectedComponent_;
    QString selectedWorker_;

    QStringList nodes_;
    QStringList components_;
    QStringList workers_;

    QString typedExperimentName_;
    QStringListModel* experimentsModel_;
    QCompleter* experimentsCompleter_;
    QMultiHash<QString, AggServerResponse::ExperimentRun> experimentRuns_;

};

#endif // CHARTINPUTPOPUP_H
