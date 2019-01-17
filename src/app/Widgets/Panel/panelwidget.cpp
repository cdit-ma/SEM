#include "panelwidget.h"

#ifdef _WIN32
    #define NOMINMAX
#endif //_WIN32

#include <QGraphicsLinearLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QDateTime>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QFileDialog>
#include <QStandardPaths>
#include <QImageWriter>
#include <QMessageBox>

#include "../../theme.h"
#include "../../Controllers/WindowManager/windowmanager.h"
#include "../Windows/mainwindow.h"
#include "../DockWidgets/defaultdockwidget.h"
#include "../Charts/Timeline/Axis/axiswidget.h"
#include "../Charts/Series/dataseries.h"

#define PANEL_OPACITY 248
#define TAB_WIDTH 100

/**
 * @brief PanelWidget::PanelWidget
 * @param parent
 */
PanelWidget::PanelWidget(QWidget *parent)
    : QFrame(parent)
{
    defaultActiveAction = 0;

    setupLayout();

    //testDataSeries();
    //testNewTimelineView();

    //testWidgets();

    //constructBigDataChart();
    //constructCustomChartView();
    //constructSizeTestTab();

    connect(Theme::theme(), &Theme::theme_Changed, this, &PanelWidget::themeChanged);
    themeChanged();

    timer = new QTimer(this);
    timer->setInterval(2000);
    //connect(timer, &QTimer::timeout, this, &PanelWidget::handleTimeout);

    tabsActionGroup = new QActionGroup(this);
    tabsActionGroup->setExclusive(true);

    setAttribute(Qt::WA_NoMousePropagation);

    // set the default active tab
    if (defaultActiveAction) {
        defaultActiveAction->trigger();
    }


}


/**
 * @brief PanelWidget::addTab
 * @param title
 * @param iconPath
 * @param iconName
 * @return
 */
QAction* PanelWidget::addTab(QString title, QString iconPath, QString iconName)
{
    return addTab(title, new QWidget(this), iconPath, iconName);
}


/**
 * @brief PanelWidget::addTab
 * @param title
 * @param widget
 * @param iconPath
 * @param iconName
 * @return
 */
QAction* PanelWidget::addTab(QString title, QWidget* widget, QString iconPath, QString iconName)
{
    if (widget) {

        QAction* action = tabBar->addAction(title);
        action->setToolTip(title);
        action->setCheckable(true);
        connect(action, &QAction::triggered, this, &PanelWidget::activeTabChanged);

        if (iconPath.isEmpty() || iconName.isEmpty()) {
            iconPath = "Icons";
            iconName = "dotsInCircle";
            //iconName = "circleHalo";
        }

        // TODO - Setup icons that ignore toggle state colouring
        // Temporary fix - pre-setup these icons
        Theme* theme = Theme::theme();
        theme->setIconToggledImage("ToggleIcons", title, iconPath, iconName, iconPath, iconName);
        updateIcon(action, "ToggleIcons", title);

        tabStack->addWidget(widget);
        tabWidgets.insert(action, widget);

        QAction* menuAction = tabsMenu->addAction(action->text());
        menuAction->setCheckable(true);
        menuAction->setChecked(true);
        tabMenuActions[menuAction] = action;

        if (!tabsActionGroup) {
            tabsActionGroup = new QActionGroup(this);
            tabsActionGroup->setExclusive(true);
        }

        int maxWidth = qMax(fontMetrics().width(title) + 40, TAB_WIDTH);
        tabsActionGroup->addAction(action);
        tabBar->widgetForAction(action)->setFixedWidth(maxWidth);
        return action;
    }

    return 0;
}


/**
 * @brief PanelWidget::isMinimised
 * @return
 */
bool PanelWidget::isMinimised()
{
    return minimiseAction->isChecked();
}


/**
 * @brief PanelWidget::constructEventsView
 */
void PanelWidget::constructEventsView()
{
    TimelineChartView* view = new TimelineChartView(this);
    view->setActiveEventKinds({TIMELINE_DATA_KIND::PORT_LIFECYCLE, TIMELINE_DATA_KIND::WORKLOAD, TIMELINE_DATA_KIND::CPU_UTILISATION, TIMELINE_DATA_KIND::MEMORY_UTILISATION});
    connectChartViewToAggreagtionProxy(view);
    defaultActiveAction = addTab("Events", view);
    defaultActiveAction->trigger();
}


void PanelWidget::testDataSeries()
{
    QList<QPointF> points;
    points.append(QPointF(0, 9));
    points.append(QPointF(3, 7));
    points.append(QPointF(6, 11));
    points.append(QPointF(8, 6));
    points.append(QPointF(10, 8));

    //MEDEA::DataSeries* ds = new MEDEA::DataSeries();
    //ds->addPoints(points);
}


void PanelWidget::testWidgets()
{
    QWidget* w = new QWidget(this);
    QWidget* filler = new QWidget(this);
    QWidget* filler2 = new QWidget(this);
    filler2->setFixedHeight(5);
    int margin = 5;

    AxisSlider* axis = new AxisSlider(Qt::Horizontal, Qt::AlignBottom, this);
    AxisDisplay* display = new AxisDisplay(axis, this);
    display->setAxisLineVisible(true);

    QVBoxLayout* layout = new QVBoxLayout(w);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setContentsMargins(margin, margin, margin, margin);
    layout->addWidget(filler, 1);
    layout->addWidget(display);
    layout->addWidget(filler2);
    layout->addWidget(axis);

    addTab("Axis", w);

    AxisWidget* axisWidget_b = new AxisWidget(Qt::Horizontal, Qt::AlignBottom, this);
    AxisWidget* axisWidget_t = new AxisWidget(Qt::Horizontal, Qt::AlignTop, this);
    AxisWidget* axisWidget_l = new AxisWidget(Qt::Vertical, Qt::AlignLeft, this);
    AxisWidget* axisWidget_r = new AxisWidget(Qt::Vertical, Qt::AlignRight, this);
    QWidget* w3 = new QWidget(this);
    QWidget* filler3 = new QWidget(this);
    filler3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    filler3->hide();

    QWidget* tempView = new QWidget(this);
    tempView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QGridLayout* layout3 = new QGridLayout(w3);
    layout3->setMargin(0);
    layout3->setSpacing(0);
    layout3->setContentsMargins(margin, margin, margin, margin);
    layout3->addWidget(axisWidget_t, 0, 1);
    layout3->addWidget(axisWidget_l, 1, 0);
    layout3->addWidget(tempView, 1, 1);
    layout3->addWidget(axisWidget_r, 1, 2);
    layout3->addWidget(axisWidget_b, 2, 1);

    addTab("AxisW", w3);
}


void PanelWidget::testNewTimelineView()
{
    TimelineChartView* view = new TimelineChartView(this);
    view->setActiveEventKinds({TIMELINE_DATA_KIND::DATA});
    defaultActiveAction = addTab("Entities", view);
    defaultActiveAction->trigger();

    if (viewController) {
        connect(viewController, &ViewController::vc_viewItemConstructed, view, &TimelineChartView::viewItemConstructed);
        connect(viewController, &ViewController::vc_viewItemDestructing, view, &TimelineChartView::viewItemDestructed);
    }
}


// EXAMPLES
void PanelWidget::constructBigDataChart()
{
    QFile file("/Sample Data/SpeciesA_Isolate1-1_110131.txt");
    QString errorStr;
    if (!file.open(QIODevice::ReadOnly)) {
        errorStr = "File ERROR: " + file.errorString();
    }

    QTextStream in(&file);
    QList<QPointF> textDataPoints;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(" ");
        if (fields.count() == 2) {
            QPointF p(fields.at(0).toFloat(), fields.at(1).toFloat());
            textDataPoints.append(p);
        }
    }
    file.close();

    //qDebug() << errorStr;
    //qDebug() << "#: " << textDataPoints.count();
}


void PanelWidget::constructCustomChartView()
{
    QGraphicsView* view = new QGraphicsView(new QGraphicsScene);
    defaultActiveAction = addTab("Custom", view);

    QChart* c1 = new QChart;
    QChart* c2 = new QChart;
    QChart* c3 = new QChart;

    QLineSeries* line = new QLineSeries;
    QSplineSeries* spline = new QSplineSeries;
    QScatterSeries* scatter = new QScatterSeries;

    c1->addSeries(line);
    c2->addSeries(spline);
    c3->addSeries(scatter);

    c1->createDefaultAxes();
    c2->createDefaultAxes();
    c3->createDefaultAxes();

    view->scene()->addItem(c1);
    view->scene()->addItem(c2);
    view->scene()->addItem(c3);

    int viewWidth = 1200; //view->viewport()->rect().width();
    c1->setMinimumSize(viewWidth / 3, 500);
    c2->setMinimumSize(viewWidth / 3, 500);
    c3->setMinimumSize(viewWidth / 3, 500);

    c2->moveBy(500, 0);
    c3->moveBy(1000, 0);
}


void PanelWidget::constructSizeTestTab()
{
    QWidget* w = new QWidget(this);
    QLabel* w1 = new QLabel("HELLO WORLD", this);
    QWidget* w2 = new QWidget(this);

    w1->setStyleSheet("background:red;");
    w2->setStyleSheet("background:blue;");

    //w1->setFixedWidth(150);

    QHBoxLayout* layout = new QHBoxLayout(w);
    layout->addWidget(w1);
    layout->addWidget(w2, 1);
    defaultActiveAction = addTab("SIZE", w);
}


void PanelWidget::setViewController(ViewController *vc)
{
    if (!vc)
        return;

    viewController = vc;

    // connect to AggregationProxy
    if (chartPopup) {
        connect(&vc->getAggregationProxy(), &AggregationProxy::setChartUserInputDialogVisible, chartPopup, &ChartInputPopup::setPopupVisible);
        connect(&vc->getAggregationProxy(), &AggregationProxy::requestedExperimentRuns, chartPopup, &ChartInputPopup::populateExperimentRuns);
        connect(&vc->getAggregationProxy(), &AggregationProxy::requestedExperimentState, chartPopup, &ChartInputPopup::receivedExperimentState);
        connect(chartPopup, &ChartInputPopup::requestExperimentRuns, &vc->getAggregationProxy(), &AggregationProxy::RequestExperimentRuns);
        connect(chartPopup, &ChartInputPopup::requestExperimentState, &vc->getAggregationProxy(), &AggregationProxy::RequestExperimentState);
        connect(chartPopup, &ChartInputPopup::requestEvents, &vc->getAggregationProxy(), &AggregationProxy::RequestEvents);
        //connect(chartPopup, &ChartInputPopup::setChartTitle, this, &PanelWidget::setActiveTabTitle);
    }
    connect(this, &PanelWidget::reloadTimelineEvents, &viewController->getAggregationProxy(), &AggregationProxy::ReloadRunningExperiments);

    //constructEventsView();
    //testNewTimelineView();
}


/**
 * @brief PanelWidget::themeChanged
 */
void PanelWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet("QFrame{ margin: 0px; padding: 0px; }");

    tabBar->setIconSize(theme->getIconSize());
    tabBar->setStyleSheet(theme->getTabbedToolBarStyleSheet() +
                          "QToolBar QToolButton{ padding-right: 7px; }");

    titleBar->setIconSize(theme->getIconSize());
    titleBar->setStyleSheet(theme->getDockTitleBarStyleSheet(false, "QToolBar") +
                            "QToolBar {"
                            "background:" + theme->getDisabledBackgroundColorHex() + ";"
                            "padding-bottom: 0px;"
                            "margin: 0px;"
                            "border: 0px;"
                            "border-top: 2px solid " + theme->getBackgroundColorHex() + ";"
                            "}"
                            "QToolBar QToolButton::!hover{ background: rgba(0,0,0,0); }");

    QColor bgColor = theme->getBackgroundColor();
    bgColor.setAlpha(PANEL_OPACITY);
    tabStack->setStyleSheet("background:" + theme->QColorToHex(bgColor) + ";"
                            "border: 0px;"
                            "margin: 0px;"
                            "padding: 0px;");

    playPauseAction->setIcon(theme->getIcon("ToggleIcons", "playPause"));
    snapShotAction->setIcon(theme->getIcon("Icons", "camera"));
    popOutActiveTabAction->setIcon(theme->getIcon("Icons", "popOut"));
    tabsMenuAction->setIcon(theme->getIcon("Icons", "list"));
    popOutAction->setIcon(theme->getIcon("Icons", "arrowLineLeft"));
    minimiseAction->setIcon(theme->getIcon("ToggleIcons", "arrowVertical"));
    closeAction->setIcon(theme->getIcon("Icons", "cross"));

    refreshDataAction->setIcon(theme->getIcon("Icons", "refresh"));

    for (auto action : tabBar->actions()) {
        QString path = action->property("iconPath").toString();
        QString name = action->property("iconName").toString();
        if (!path.isEmpty() && !name.isEmpty()) {
            updateIcon(action, path, name, false);
        }
    }
}


/**
 * @brief PanelWidget::activeTabChanged
 */
void PanelWidget::activeTabChanged()
{
    QAction* activeTabAction = qobject_cast<QAction*>(sender());
    if (activeTabAction) {
        /*QWidget* tabWidget = tabWidgets.value(activeTabAction, 0);
        // TODO - This is temporary; testing live data stream
        if (tabWidget) {
            playPauseAction->setVisible(activeTabAction->text() == "Test");
            tabStack->setCurrentWidget(tabWidget);
        }*/
    }
}


/**
 * @brief PanelWidget::tabMenuTriggered
 * @param action
 */
void PanelWidget::tabMenuTriggered(QAction* action)
{
    QAction* tabAction = tabMenuActions.value(action, 0);
    if (!tabAction)
        return;

    // show/hide action's related tab and widget
    bool checked = action->isChecked();
    tabAction->setVisible(checked);

    if (checked) {
        // if no tabs were shown previously, set this newly checked tab active
        if (hiddenTabs == tabsActionGroup->actions().count()) {
            tabAction->trigger();
        }
        tabsActionGroup->setVisible(true);
        popOutActiveTabAction->setVisible(true);
        hiddenTabs--;
    } else {
        // check if the hidden tab is currently the active tab
        if (tabAction == tabsActionGroup->checkedAction()) {
            activateNewTab(tabAction);
        }
        hiddenTabs++;
        // if all tabs are hidden, minimise the panel
        if (hiddenTabs == tabsActionGroup->actions().count()) {
            popOutActiveTabAction->setVisible(false);
            tabsActionGroup->setVisible(false);
            minimiseAction->triggered(true);
            minimiseAction->setChecked(true);
        }
    }
}


/**
 * @brief PanelWidget::setActiveTabTitle
 * @param title
 */
void PanelWidget::setActiveTabTitle(QString title)
{
    if (tabsActionGroup) {
        auto activeAction = tabsActionGroup->checkedAction();
        if (activeAction) {
            activeAction->setText(title);
            activeAction->setToolTip(title);
            int maxWidth = qMax(fontMetrics().width(title) + 40, TAB_WIDTH);
            tabBar->widgetForAction(activeAction)->setFixedWidth(maxWidth);
        }
    }
}


/**
 * @brief PanelWidget::minimisePanel
 * @param checked
 */
void PanelWidget::minimisePanel(bool checked)
{
    /*if (!checked) {
        // if all the tabs are currently hidden, don't allow the panel to be maximised
        if (hiddenTabs == tabsActionGroup->actions().count()) {
            minimiseAction->setChecked(true);
            return;
        }
    }*/
    tabStack->setVisible(!checked);
    emit minimiseTriggered(checked);
}


/**
 * @brief PanelWidget::closePanel
 */
void PanelWidget::closePanel()
{
    setVisible(false);
    emit closeTriggered();
}


/**
 * @brief PanelWidget::snapShotPanel
 */
void PanelWidget::snapShotPanel()
{
    QAction* activeAction = tabsActionGroup->checkedAction();
    if (!activeAction)
        return;

    QWidget* tabWidget = tabWidgets.value(activeAction, 0);
    if (!tabWidget)
        return;

    QPixmap widgetPixmap = tabWidget->grab();
    QString initialPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    const QString format = "png";
    if (initialPath.isEmpty())
        initialPath = QDir::currentPath();
    initialPath += tr("/untitled.") + format;

    QFileDialog fileDialog(this, tr("Save As"), initialPath);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setDirectory(initialPath);

    QStringList mimeTypes;
    foreach (const QByteArray &bf, QImageWriter::supportedMimeTypes())
        mimeTypes.append(QLatin1String(bf));
    fileDialog.setMimeTypeFilters(mimeTypes);
    fileDialog.selectMimeTypeFilter("image/" + format);
    fileDialog.setDefaultSuffix(format);

    if (fileDialog.exec() != QDialog::Accepted)
        return;

    const QString fileName = fileDialog.selectedFiles().first();
    if (!widgetPixmap.save(fileName))
        QMessageBox::warning(this, tr("Save Error"), tr("The image could not be saved to \"%1\".")
                             .arg(QDir::toNativeSeparators(fileName)));
}


/**
 * @brief PanelWidget::popOutPanel
 */
void PanelWidget::popOutPanel()
{

}


/**
 * @brief PanelWidget::popOutActiveTab
 */
void PanelWidget::popOutActiveTab()
{
    QAction* activeTab = tabsActionGroup->checkedAction();
    if (!activeTab)
        return;

    WindowManager* manager = WindowManager::manager();
    DefaultDockWidget* dockWidget = manager->constructDockWidget(activeTab->text(), manager->getMainWindow());
    if (!manager->reparentDockWidget(dockWidget)) {
        manager->destructDockWidget(dockWidget);
        return;
    }

    QWidget* tabWidget = tabWidgets.value(activeTab, 0);
    dockWidget->setWidget(tabWidget);

    // remove tab and related actions from this panel
    removeTab(activeTab, false);

    // we need to set another tab active
    activateNewTab(activeTab);
}


/**
 * @brief PanelWidget::requestData
 */
void PanelWidget::requestData()
{
    emit reloadTimelineEvents();
}


/**
 * @brief PanelWidget::handleTimeout
 */
void PanelWidget::handleTimeout()
{
    dataIndex++;
    if (dataIndex >= sampleDataY.count()) {
        dataIndex = 0;
    }

    QDateTime dt = QDateTime::currentDateTime();
    qreal value = sampleDataY.at(dataIndex);
    QPointF nextPoint(dt.toMSecsSinceEpoch(), value);
    nextDataPoints.append(nextPoint);

    if (playPauseAction->isChecked()) {
        testSeries->append(nextDataPoints);
        nextDataPoints.clear();
        //Chart* tChart = (Chart*)testSeries->chart();
        //tChart->setDataMaxX(dt);
        //tChart->setAxisRange(Qt::Horizontal, minDateTime, dt);
        minDateTime.setMSecsSinceEpoch(minDateTime.toMSecsSinceEpoch() + timer->interval());
    }

    // NOTE: The values put into this scroll function are in pixels
    //chart->scroll(nextX, 0);
}


/**
 * @brief PanelWidget::playPauseToggled
 * @param checked
 */
void PanelWidget::playPauseToggled(bool checked)
{
    if (checked && !timer->isActive()) {
        minDateTime.setMSecsSinceEpoch(QDateTime::currentMSecsSinceEpoch() - 5000);
        timer->start();
    }
}


/**
 * @brief PanelWidget::removeTab
 * @param tabAction
 * @param deleteWidget
 */
void PanelWidget::removeTab(QAction* tabAction, bool deleteWidget)
{
    if (tabAction) {
        // remove action from the tab bar
        tabBar->removeAction(tabAction);

        // remove related widget from the stack and the tab widget hash
        QWidget* w = tabWidgets.take(tabAction);
        tabStack->removeWidget(w);

        // remove related action from the tabs menu and the menu actions hash
        QAction* menuAction = tabMenuActions.key(tabAction);
        tabsMenu->removeAction(menuAction);
        tabMenuActions.remove(menuAction);

        // remove action from the tabs action group
        tabsActionGroup->removeAction(tabAction);

        if (deleteWidget) {
            w->deleteLater();
        }

        tabAction->deleteLater();
        menuAction->deleteLater();
    }
}


/**
 * @brief PanelWidget::activateNewTab
 * @param previouslyActivatedTab
 */
void PanelWidget::activateNewTab(QAction* previouslyActivatedTab)
{
    for (QAction* tab : tabsActionGroup->actions()) {
        if (tab == previouslyActivatedTab) {
            continue;
        }
        if (tab->isVisible()) {
            tab->trigger();
            return;
        }
    }
}


/**
 * @brief PanelWidget::setupLayout
 */
void PanelWidget::setupLayout()
{
    tabBar = new QToolBar(this);
    tabBar->setFloatable(false);
    tabBar->setMovable(false);
    tabBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tabBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    titleBar = new QToolBar(this);
    titleBar->setFloatable(false);
    titleBar->setMovable(false);
    titleBar->addWidget(tabBar);

    // Tab specific action
    {
        playPauseAction = titleBar->addAction("Play/Pause Data Stream");
        playPauseAction->setCheckable(true);
        playPauseAction->setChecked(false);
        playPauseAction->setVisible(false);
    }

    refreshDataAction = titleBar->addAction("Refresh Data");
    connect(refreshDataAction, &QAction::triggered, [=]() {
        requestData();
    });

    refreshDataAction->setVisible(false); // hide for now
    titleBar->addSeparator();

    snapShotAction = titleBar->addAction("Take Chart Snapshot");
    popOutActiveTabAction = titleBar->addAction("Popout Tab");

    tabsMenu = new QMenu(this);
    connect(tabsMenu, &QMenu::triggered, this, &PanelWidget::tabMenuTriggered);

    tabsMenuAction = titleBar->addAction("Tabs");
    tabsMenuAction->setMenu(tabsMenu);
    tabsMenuAction->setVisible(false); // hide for now

    QToolButton* menuButton = (QToolButton*) titleBar->widgetForAction(tabsMenuAction);
    menuButton->setPopupMode(QToolButton::InstantPopup);

    //titleBar->addSeparator();

    minimiseAction = titleBar->addAction("Minimise/Maximise");
    minimiseAction->setCheckable(true);

    popOutAction = titleBar->addAction("Show Panel Dialog");
    closeAction = titleBar->addAction("Close");

    // TODO: Not using this action currently - hide for now
    popOutAction->setVisible(false);

    tabStack = new QStackedWidget(this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(titleBar);
    layout->addWidget(tabStack, 1);

    // connect titleBar actions
    connect(popOutActiveTabAction, &QAction::triggered, this, &PanelWidget::popOutActiveTab);
    connect(minimiseAction, &QAction::triggered, this, &PanelWidget::minimisePanel);
    connect(closeAction, &QAction::triggered, this, &PanelWidget::closePanel);
    connect(snapShotAction, &QAction::triggered, this, &PanelWidget::snapShotPanel);
    connect(playPauseAction, &QAction::toggled, this, &PanelWidget::playPauseToggled);

    chartPopup = new ChartInputPopup(this);
}


/**
 * @brief PanelWidget::updateIcon
 * @param action
 * @param iconPath
 * @param iconName
 * @param newIcon
 */
void PanelWidget::updateIcon(QAction* action, QString iconPath, QString iconName, bool newIcon)
{
    if (action) {
        if (newIcon) {
            action->setProperty("iconPath", iconPath);
            action->setProperty("iconName", iconName);
        }
        action->setIcon(Theme::theme()->getIcon(iconPath, iconName));
    }
}


/**
 * @brief PanelWidget::connectChartViewToAggreagtionProxy
 * @param view
 */
void PanelWidget::connectChartViewToAggreagtionProxy(TimelineChartView* view)
{
    if (view && viewController) {
        for (auto kind : view->getActiveEventKinds()) {
            switch (kind) {
            case TIMELINE_DATA_KIND::PORT_LIFECYCLE:
                //connect(&viewController->getAggregationProxy(), &AggregationProxy::receivedPortLifecycleEvent, view, &TimelineChartView::receivedRequestedEvent);
                break;
            case TIMELINE_DATA_KIND::WORKLOAD:
                //connect(&viewController->getAggregationProxy(), &AggregationProxy::receivedWorkloadEvent, view, &TimelineChartView::receivedRequestedEvent);
                break;
            case TIMELINE_DATA_KIND::CPU_UTILISATION:
                //connect(&viewController->getAggregationProxy(), &AggregationProxy::receivedCPUUtilisationEvent, view, &TimelineChartView::receivedRequestedEvent);
                break;
            case TIMELINE_DATA_KIND::MEMORY_UTILISATION:
                //connect(&viewController->getAggregationProxy(), &AggregationProxy::receivedMemoryUtilisationEvent, view, &TimelineChartView::receivedRequestedEvent);
                break;
            }
        }
        connect(&viewController->getAggregationProxy(), &AggregationProxy::clearPreviousEvents, view, &TimelineChartView::clearSeriesEvents);
        connect(&viewController->getAggregationProxy(), &AggregationProxy::receivedAllEvents, view, &TimelineChartView::updateTimelineChart);
        connect(viewController, &ViewController::vc_viewItemsInChart, view, &TimelineChartView::viewEventsForItems);
    }
}
