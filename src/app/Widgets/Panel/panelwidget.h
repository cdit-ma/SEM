#ifndef PANELWIDGET_H
#define PANELWIDGET_H

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QFrame>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QStackedWidget>

#include "../../Controllers/ViewController/viewcontroller.h"
#include "../Charts/Timeline/Chart/timelinechartview.h"
#include "../Charts/Timeline/chartinputpopup.h"

namespace QtCharts{
    class QLineSeries;
    class QCategoryAxis;
}

using namespace QtCharts;

class PanelWidget : public QFrame
{
    Q_OBJECT

public:
    explicit PanelWidget(QWidget* parent = 0);

    QAction* addTab(QString title, QString iconPath = "", QString iconName = "");
    QAction* addTab(QString title, QWidget* widget, QString iconPath = "", QString iconName = "");

    void addEventsTab();

    bool isMinimised();

    void constructEventsView();
    void constructPortLifecycleEventsView();
    void constructWorkloadEventsView();
    void constructCPUEventsView();

    void testDataSeries();
    void testWidgets();
    void testNewTimelineView();

    void constructBigDataChart();
    void constructCustomChartView();
    void constructSizeTestTab();

    void setViewController(ViewController* vc);

signals:
    void minimiseTriggered(bool checked);
    void closeTriggered();

    void reloadTimelineEvents();

public slots:
    void themeChanged();
    void activeTabChanged();

    void tabMenuTriggered(QAction* action);
    void setActiveTabTitle(QString title);

    void minimisePanel(bool checked);
    void closePanel();
    void snapShotPanel();
    void popOutPanel();
    void popOutActiveTab();

    void requestData();

    void handleTimeout();
    void playPauseToggled(bool checked);

private:
    void removeTab(QAction* tabAction, bool deleteWidget = true);
    void activateNewTab(QAction* previouslyActivatedTab);

    void setupLayout();
    void updateIcon(QAction* action, QString iconPath, QString iconName, bool newIcon = true);

    void connectChartViewToAggreagtionProxy(TimelineChartView* view);

    ViewController* viewController = 0;
    ChartInputPopup* chartPopup = 0;

    QToolBar* tabBar = 0;
    QToolBar* titleBar = 0;

    QAction* minimiseAction = 0;
    QAction* closeAction = 0;
    QAction* popOutAction = 0;

    QAction* snapShotAction = 0;
    QAction* popOutActiveTabAction = 0;
    QAction* playPauseAction = 0;

    QAction* refreshDataAction = 0;

    QAction* tabsMenuAction = 0;
    QMenu* tabsMenu = 0;
    QHash<QAction*, QAction*> tabMenuActions;
    int hiddenTabs = 0;

    QStackedWidget* tabStack = 0;
    QActionGroup* tabsActionGroup = 0;
    QHash<QAction*, QWidget*> tabWidgets;

    QTimer* timer;
    QChart* testChart;
    QChartView* testView;
    QLineSeries* testSeries;

    QChart* chart;
    QChartView* chartView;
    QCategoryAxis* categoryAxis;

    QAction* defaultActiveAction = 0;
    QDateTime minDateTime;

    QList<int> sampleDataY;
    QList<QPointF> sampleDataPoints;
    QList<QPointF> nextDataPoints;
    int dataIndex;
};

#endif // PANELWIDGET_H
