#ifndef PANELWIDGET_H
#define PANELWIDGET_H

#include <QFrame>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QStackedWidget>

#include "../../Controllers/ViewController/viewcontroller.h"
#include "../Charts/Timeline/Chart/timelinechartview.h"
#include "../Charts/Timeline/chartinputpopup.h"

class PanelWidget : public QFrame
{
    Q_OBJECT

public:
    explicit PanelWidget(QWidget* parent = nullptr);

    QAction* addTab(QString title, QString iconPath = "", QString iconName = "");
    QAction* addTab(QString title, QWidget* widget, QString iconPath = "", QString iconName = "");

    bool isMinimised() const;

    void constructEventsView();
    void constructPortLifecycleEventsView();
    void constructWorkloadEventsView();
    void constructCPUEventsView();
    void constructMemoryEventsView();

    void setViewController(ViewController* vc);

signals:
    void minimiseTriggered(bool checked);
    void closeTriggered();

    void reloadTimelineEvents();

public slots:
    void themeChanged();
    void activeTabChanged();

    void tabMenuTriggered(QAction* action);
    void setActiveTabTitle(const QString& title);

    void minimisePanel(bool checked);
    void closePanel();
    void snapShotPanel();
    void popOutActiveTab();
    void clearActiveTab();

    void requestData();

    void handleTimeout();
    void playPauseToggled(bool checked);

private:
    void removeTab(QAction* tabAction, bool deleteWidget = true);
    void activateNewTab(QAction* previouslyActivatedTab);

    void setupLayout();
    void updateIcon(QAction* action, const QString& iconPath, const QString& iconName, bool newIcon = true);

    void connectChartViewToAggreagtionProxy(TimelineChartView* view);

    ViewController* viewController = nullptr;
    ChartInputPopup* chartPopup = nullptr;

    QToolBar* tabBar = nullptr;
    QToolBar* titleBar = nullptr;

    QAction* minimiseAction = nullptr;
    QAction* closeAction = nullptr;
    QAction* snapShotAction = nullptr;
    QAction* popOutAction = nullptr;
    QAction* popOutActiveTabAction = nullptr;
    QAction* clearActiveTabAction = nullptr;

    QAction* playPauseAction = nullptr;
    QAction* refreshDataAction = nullptr;

    QAction* tabsMenuAction = nullptr;
    QMenu* tabsMenu = nullptr;
    QHash<QAction*, QAction*> tabMenuActions;
    int hiddenTabs = 0;

    QStackedWidget* tabStack = nullptr;
    QActionGroup* tabsActionGroup = nullptr;
    QHash<QAction*, QWidget*> tabWidgets;

    QTimer* timer = nullptr;

    QAction* defaultActiveAction = nullptr;
    QDateTime minDateTime;

    QList<int> sampleDataY;
    QList<QPointF> sampleDataPoints;
    QList<QPointF> nextDataPoints;
    int dataIndex;

    int tabID_;
    static int tab_ID;
};

#endif // PANELWIDGET_H