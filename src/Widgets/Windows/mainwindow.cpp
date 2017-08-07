#include "mainwindow.h"
#include "../DockWidgets/viewdockwidget.h"
#include "../DockWidgets/nodeviewdockwidget.h"
#include "../DockWidgets/tooldockwidget.h"


#include "../../Controllers/SelectionController/selectioncontroller.h"
#include "../../Controllers/SettingsController/settingscontroller.h"

#include "../../Widgets/ViewManager/viewmanagerwidget.h"
#include "../../Widgets/Jenkins/jenkinsjobmonitorwidget.h"
#include "../../Widgets/optiongroupbox.h"
#include "../../Views/Notification/notificationobject.h"
#include "../../theme.h"

#include "../../Utils/filtergroup.h"
#include "../../Widgets/customgroupbox.h"

#include <QDebug>
#include <QHeaderView>
#include <QPushButton>
#include <QMenuBar>
#include <QDateTime>
#include <QApplication>
#include <QStringBuilder>
#include <QStringListModel>
#include <QTabWidget>

#define TOOLBAR_HEIGHT 32


/**
 * @brief MedeaMainWindow::MedeaMainWindow
 * @param vc
 * @param parent
 */
MainWindow::MainWindow(ViewController *vc, QWidget* parent):BaseWindow(parent, BaseWindow::MAIN_WINDOW)
{
    viewController = vc;
    initializeApplication();
    setContextMenuPolicy(Qt::NoContextMenu);
    innerWindow = WindowManager::constructCentralWindow("Main Window");
    setCentralWidget(innerWindow);

    setupTools();
    setupInnerWindow();
    setupJenkinsManager();
    setViewController(vc);

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(WindowManager::manager(), SIGNAL(activeViewDockWidgetChanged(ViewDockWidget*,ViewDockWidget*)), this, SLOT(activeViewDockWidgetChanged(ViewDockWidget*, ViewDockWidget*)));

    SettingsController* s = SettingsController::settings();

    auto outer_geo = s->getSetting(SETTINGS::WINDOW_OUTER_GEOMETRY).toByteArray();
    if(!outer_geo.isEmpty()){
        restoreGeometry(outer_geo);
    }else{
        //setMinimumSize(1200, 800);
    }

    setModelTitle();
    themeChanged();
    toggleWelcomeScreen(true);
    show();
    
    /*
    auto group_box = new OptionGroupBox("Test group", this);
    for(auto s : getNotificationSeverities()){
        group_box->addOption(static_cast<uint>(s), getSeverityString(s), "Icons", getSeverityIcon(s));
    }
    group_box->addOption("HELLO", "HELLO", "Icons", "circle");
    auto vals = group_box->getOptions<NOTIFICATION_SEVERITY>();
    for (auto v : vals){
        qCritical() << getSeverityString(v);
    }

     auto vals2 = group_box->getOptions<QVariant>();
    for (auto v : vals2){
        qCritical() << v;
    }
    group_box->setExclusive(true);

    QDialog* d = new QDialog(this);
    d->setModal(false);
    QVBoxLayout* l = new QVBoxLayout(d);
    l->addWidget(group_box);
    d->setStyleSheet("background:gray;");
    d->show();
    d->exec();
    */
}


/**
 * @brief MedeaMainWindow::~MedeaMainWindow
 */
MainWindow::~MainWindow()
{
    SettingsController::teardownSettings();
    Theme::teardownTheme();
}


/**
 * @brief MedeaMainWindow::setViewController
 * @param vc
 */
void MainWindow::setViewController(ViewController *vc)
{
    viewController = vc;

    SelectionController* controller = vc->getSelectionController();
    ActionController* actionController = vc->getActionController();

    //connect(viewController, &ViewController::vc_backgroundProcess, NotificationManager::manager(), &NotificationManager::backgroundProcess);
    connect(viewController, &ViewController::mc_projectModified, this, &MainWindow::setWindowModified);
    connect(viewController, &ViewController::vc_projectPathChanged, this, &MainWindow::setModelTitle);
    connect(viewController, &ViewController::vc_showWelcomeScreen, this, &MainWindow::toggleWelcomeScreen);

    connect(controller, &SelectionController::itemActiveSelectionChanged, tableWidget, &DataTableWidget::itemActiveSelectionChanged);

    if (actionController) {
        connect(actionController, &ActionController::recentProjectsUpdated, this, &MainWindow::recentProjectsUpdated);
        connect(actionController->getRootAction("Root_Search"), SIGNAL(triggered(bool)), this, SLOT(popupSearch()));
    }
}


/**
 * @brief MedeaMainWindow::showCompletion
 * @param list
 */
void MainWindow::updateSearchSuggestions(QStringList list)
{
    searchCompleterModel->setStringList(list);
}


/**
 * @brief MedeaMainWindow::searchEntered
 */
void MainWindow::searchEntered()
{
    QString query = searchBar->text();

    if (!query.isEmpty()) {

        //searchPanel->loading(true);
        qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
        searchPanel->searchResults(query, viewController->getSearchResults(query));
        qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();
        //searchPanel->loading(false);
        qCritical() << "searchEntered in: " <<  timeFinish - timeStart << "MS";

        // make sure that the search panel dock widget is visible and that its window is raised
        QWidget* parentWindow = searchDockWidget->window();
        if (!searchDockWidget->isVisible()) {
            // check if its window is minimized
            if (parentWindow->isMinimized()) {
                parentWindow->showNormal();
            }
            // check if it's not shown
            if (!searchDockWidget->isVisible()) {
                searchDockWidget->req_Visible(searchDockWidget->getID(), true);
            }
        }

        // raise and activate the dock's parent window
        parentWindow->raise();
        searchDockWidget->activateWindow();

        // raise the dock widget - this brings the dock widget to the front if it's tabbed
        searchDockWidget->raise();
    }
}


void MainWindow::popupLatestNotification(){
    notificationPopup->hide();
    notificationTimer->stop();

    if (!welcomeScreenOn) {
        auto notification = NotificationManager::manager()->getLastNotificationItem();
        if(notification){
            notificationLabel->setText(notification->description());
            auto pixmap = Theme::theme()->getImage(notification->iconPath(), notification->iconName(), QSize(32,32), getSeverityColor(notification->severity()));
            
            if (pixmap.isNull()) {
                pixmap = Theme::theme()->getImage("Icons", "circleInfo", QSize(32,32), getSeverityColor(notification->severity()));
            }

            notificationIconLabel->setPixmap(pixmap);
            notificationPopup->setSize(notificationWidget->sizeHint().width() + 15, notificationWidget->sizeHint().height() + 10);
            moveWidget(notificationPopup, 0, Qt::AlignBottom);

            notificationPopup->show();
            notificationPopup->raise();
            notificationTimer->start(5000);
        }
    }

}
/**
 * @brief MainWindow::toggleNotificationPanel
 * This toggles the visibility of the notification panel dock widget.
 * If it's already visible but its parent window is not active, activate
 * the window instead of hiding it. Otherwise, hide the dock widget.
 */
void MainWindow::toggleNotificationPanel()
{
    if (notificationDockWidget->isVisible()) {
        if (notificationDockWidget->isActiveWindow()) {
            notificationDockWidget->req_Visible(notificationDockWidget->getID(), false);
            return;
        }
    }
    ensureNotificationPanelVisible();
}


/**
 * @brief MainWindow::ensureNotificationPanelVisible
 * This ensures that the notification dock widget is visible and activated/raised.
 */
void MainWindow::ensureNotificationPanelVisible()
{
    if (!notificationDockWidget->isVisible()) {
        notificationDockWidget->req_Visible(notificationDockWidget->getID(), true);
    }
    notificationDockWidget->window()->raise();
    notificationDockWidget->activateWindow();
    notificationDockWidget->raise();
}


/**
 * @brief MedeaMainWindow::showProgressBar
 * @param show
 * @param description
 */
void MainWindow::showProgressBar(bool show, QString description)
{
    if (show && progressLabel->text() != description) {
        progressLabel->setText(description);
    }

    if (show) {
        Qt::Alignment alignment = welcomeScreenOn ? Qt::AlignBottom : Qt::AlignCenter;
        moveWidget(progressPopup, innerWindow, alignment);
    } else {
        progressBar->reset();
    }

    progressPopup->setVisible(show);
}


/**
 * @brief MedeaMainWindow::updateProgressBar
 * This updates the value in the progress bar and hides it when 100% is reached.
 * @param value
 */
void MainWindow::updateProgressBar(int value)
{
    if (progressPopup->isVisible()) {
        if (value == -1) {
            progressBar->setRange(0, 0);
        } else {
            progressBar->setRange(0, 100);
            progressBar->setValue(value);
            if (value >= 100) {
                progressPopup->setVisible(false);
            }
        }
    }
}


/**
 * @brief MainWindow::updateMenuBarSize
 * This is called when the displayed notification count is updated in the menu corner widget.
 * It updates the menubar's size if necessary.
 */
void MainWindow::updateMenuBarSize()
{
    menuBar->adjustSize();
    menuBar->updateGeometry();
}


/**
 * @brief MedeaMainWindow::resetToolDockWidgets
 */
void MainWindow::resetToolDockWidgets()
{
    resetDockWidgets();
    innerWindow->addToolBar(applicationToolbar);
    applicationToolbar->setVisible(true);
    resizeToolWidgets();
}


/**
 * @brief MedeaMainWindow::themeChanged
 */
void MainWindow::themeChanged()
{
    Theme* theme = Theme::theme();

    applicationToolbar->setStyleSheet("QToolBar{background:" % theme->getBackgroundColorHex() % "; padding:4px;} "
            " QToolBar::handle{width:16px;height:16px; background:" % theme->getAltBackgroundColorHex() % ";}"
            " QToolBar::handle:horizontal{image: url(:/Images/Icons/dotsVertical);}"
            " QToolBar::handle:vertical{image: url(:/Images/Icons/dotsHorizontal);}"
    );

    QString menuStyle = theme->getMenuStyleSheet();
    ActionController* actionController = viewController->getActionController();

    actionController->menu_file->setStyleSheet(menuStyle);
    actionController->menu_file_recentProjects->setStyleSheet(menuStyle);
    actionController->menu_edit->setStyleSheet(menuStyle);
    actionController->menu_view->setStyleSheet(menuStyle);
    actionController->menu_model->setStyleSheet(menuStyle);
    actionController->menu_jenkins->setStyleSheet(menuStyle);
    actionController->menu_help->setStyleSheet(menuStyle);
    actionController->menu_options->setStyleSheet(menuStyle);

    searchCompleter->popup()->setStyleSheet(theme->getAbstractItemViewStyleSheet() % theme->getScrollBarStyleSheet() % "QAbstractItemView::item{ padding: 2px 0px; }");
    searchPopup->setStyleSheet(theme->getPopupWidgetStyleSheet());
    searchToolbar->setStyleSheet(theme->getToolBarStyleSheet());
    searchBar->setStyleSheet(theme->getLineEditStyleSheet());
    searchButton->setIcon(theme->getIcon("Icons", "zoom"));

    progressPopup->setStyleSheet(theme->getPopupWidgetStyleSheet());
    progressBar->setStyleSheet(theme->getProgressBarStyleSheet());
    progressLabel->setStyleSheet("background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() + ";");

    notificationPopup->setStyleSheet(theme->getPopupWidgetStyleSheet() + "QLabel{ background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() + "; }");
    //notificationLabel->setStyleSheet("background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() + ";");

    restoreToolsButton->setIcon(theme->getIcon("Icons", "spanner"));
    restoreToolsAction->setIcon(theme->getIcon("Icons", "refresh"));

    minimap->setStyleSheet(theme->getNodeViewStyleSheet());
}


/**
 * @brief MedeaMainWindow::activeViewDockWidgetChanged
 * @param viewDock
 * @param prevDock
 */
void MainWindow::activeViewDockWidgetChanged(ViewDockWidget *viewDock, ViewDockWidget *prevDock)
{
    //Unattach old view
    if(prevDock && prevDock->isNodeViewDock()){
        auto prev_node_view = ((NodeViewDockWidget*) prevDock)->getNodeView();
        
        if(prev_node_view){
            minimap->disconnect(prev_node_view);
            prev_node_view->disconnect(minimap);
        }
    }

    NodeView* node_view = 0;

    //Attach new view
    if(viewDock && viewDock->isNodeViewDock()){
        node_view = ((NodeViewDockWidget*) viewDock)->getNodeView();
    }

    if(node_view){
        minimap->setBackgroundColor(node_view->getBackgroundColor());
        minimap->setScene(node_view->scene());
        
        connect(minimap, &NodeViewMinimap::minimap_CenterView, node_view, &NodeView::fitToScreen);
        connect(minimap, &NodeViewMinimap::minimap_Pan, node_view, &NodeView::minimap_Pan);
        connect(minimap, &NodeViewMinimap::minimap_Zoom, node_view, &NodeView::minimap_Zoom);
        connect(node_view, &NodeView::viewport_changed, minimap, &NodeViewMinimap::viewport_changed);
        node_view->update_minimap();
    }else{
        minimap->setBackgroundColor(QColor(0,0,0));
        minimap->setScene(0);
    }
}


/**
 * @brief MedeaMainWindow::popupSearch
 * This pops up and brings the search bar in focus.
 */
void MainWindow::popupSearch()
{
    emit requestSuggestions();
    moveWidget(searchPopup);
    searchPopup->show();
    searchBar->setFocus();
    searchBar->selectAll();
}

/**
 * @brief MedeaMainWindow::setModelTitle
 * @param modelTitle
 */
void MainWindow::setModelTitle(QString modelTitle)
{
    if(!modelTitle.isEmpty()){
        modelTitle = "- " % modelTitle;
    }
    QString title = "MEDEA " % modelTitle % "[*]";
    setWindowTitle(title);
}


/**
 * @brief MedeaMainWindow::initializeApplication
 */
void MainWindow::initializeApplication()
{
    //Allow Drops
    setAcceptDrops(true);

    //Set QApplication information.
    QApplication::setApplicationName("MEDEA");
    QApplication::setApplicationVersion(APP_VERSION());
    QApplication::setOrganizationName("CDIT-MA");
    QApplication::setOrganizationDomain("https://github.com/cdit-ma/");
    QApplication::setWindowIcon(Theme::theme()->getIcon("Icons", "medeaLogo"));

    QFont font("Verdana");
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPointSizeF(8.5);
    QApplication::setFont(font);

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
}


/**
 * @brief MedeaMainWindow::toggleWelcomeScreen
 * @param on
 */
void MainWindow::toggleWelcomeScreen(bool on)
{
    if (welcomeScreenOn == on) {
        return;
    }

    welcomeScreenOn = on;

    // show/hide the menu bar and close all dock widgets
    menuBar->setVisible(!on);
    setDockWidgetsVisible(!on);

    if (on) {
        holderLayout->removeWidget(welcomeScreen);
        holderLayout->addWidget(innerWindow);
        setCentralWidget(welcomeScreen);
        // hide notification toast
        notificationPopup->hide();
        notificationTimer->stop();
    } else {
        holderLayout->removeWidget(innerWindow);
        holderLayout->addWidget(welcomeScreen);
        setCentralWidget(innerWindow);
        restoreWindowState();

        //Call this after everything has loaded
        QMetaObject::invokeMethod(this, "popupLatestNotification", Qt::QueuedConnection);
    }
}


/**
 * @brief MedeaMainWindow::saveSettings
 */
void MainWindow::saveWindowState()
{
    SettingsController* s = SettingsController::settings();
    if(s){
        if(s->getSetting(SETTINGS::GENERAL_SAVE_WINDOW_ON_EXIT).toBool()){
            s->setSetting(SETTINGS::WINDOW_OUTER_GEOMETRY, saveGeometry());
            s->setSetting(SETTINGS::WINDOW_OUTER_STATE, saveState());
        }
        if(s->getSetting(SETTINGS::GENERAL_SAVE_DOCKS_ON_EXIT).toBool()){
            s->setSetting(SETTINGS::WINDOW_INNER_GEOMETRY, innerWindow->saveGeometry());
            s->setSetting(SETTINGS::WINDOW_INNER_STATE, innerWindow->saveState());
        }
    }
}

void MainWindow::restoreWindowState(){
    SettingsController* s = SettingsController::settings();
    if(s){
        auto load_dock = s->getSetting(SETTINGS::GENERAL_SAVE_DOCKS_ON_EXIT).toBool();
        auto load_window = s->getSetting(SETTINGS::GENERAL_SAVE_WINDOW_ON_EXIT).toBool();

        auto outer_geo = s->getSetting(SETTINGS::WINDOW_OUTER_GEOMETRY).toByteArray();
        auto outer_state = s->getSetting(SETTINGS::WINDOW_OUTER_STATE).toByteArray();
        auto inner_geo = s->getSetting(SETTINGS::WINDOW_INNER_GEOMETRY).toByteArray();
        auto inner_state = s->getSetting(SETTINGS::WINDOW_INNER_STATE).toByteArray();

        //Check if any are invalid
        bool invalid = outer_geo.isEmpty() || outer_state.isEmpty() || inner_geo.isEmpty() || inner_state.isEmpty();

        if(invalid){
            resetToolDockWidgets();
        }else{
            if(load_window){
                restoreState(outer_state);
            }
            if(load_dock){
                innerWindow->restoreState(inner_state);
            }
            restoreGeometry(outer_geo);
        }
    }
}


/**
 * @brief MedeaMainWindow::setupTools
 */
void MainWindow::setupTools()
{
    welcomeScreen = 0;
    menuBar = 0;
    applicationToolbar = 0;
    dockTabWidget = 0;
    tableWidget = 0;
    minimap = 0;

    searchPopup = 0;
    searchToolbar = 0;
    searchBar = 0;
    searchButton = 0;
    searchPanel = 0;
    searchCompleter = 0;
    searchCompleterModel = 0;

    progressPopup = 0;
    progressBar = 0;
    progressLabel = 0;

    notificationPopup = 0;
    notificationWidget = 0;
    notificationIconLabel = 0;
    notificationLabel = 0;
    notificationTimer = 0;
    notificationToolbar = 0;
    notificationPanel = 0;

    restoreToolsButton = 0;
    restoreToolsAction = 0;

    viewManager = 0;

    setupWelcomeScreen();
    setupMenuBar();
    setupSearchBar();
    setupProgressBar();
    setupNotificationBar();
    setupDock();
    setupToolBar();
    setupDataTable();
    setupViewManager();
    setupMinimap();
}


/**
 * @brief MedeaMainWindow::setupInnerWindow
 */
void MainWindow::setupInnerWindow()
{
    

    //Construct dockWidgets.
    NodeViewDockWidget* dwInterfaces = viewController->constructNodeViewDockWidget("Interfaces");
    NodeViewDockWidget* dwBehaviour = viewController->constructNodeViewDockWidget("Behaviour");
    NodeViewDockWidget* dwAssemblies = viewController->constructNodeViewDockWidget("Assemblies");
    NodeViewDockWidget* dwHardware = viewController->constructNodeViewDockWidget("Hardware");

    //Set each NodeView with there contained aspects
    dwInterfaces->getNodeView()->setContainedViewAspect(VIEW_ASPECT::INTERFACES);
    dwBehaviour->getNodeView()->setContainedViewAspect(VIEW_ASPECT::BEHAVIOUR);
    dwAssemblies->getNodeView()->setContainedViewAspect(VIEW_ASPECT::ASSEMBLIES);
    dwHardware->getNodeView()->setContainedViewAspect(VIEW_ASPECT::HARDWARE);

    //Set allowed areas
    dwInterfaces->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dwBehaviour->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dwAssemblies->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dwHardware->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    //Set Icons
    dwInterfaces->setIcon("EntityIcons", "InterfaceDefinitions");
    dwBehaviour->setIcon("EntityIcons", "BehaviourDefinitions");
    dwAssemblies->setIcon("EntityIcons", "AssemblyDefinitions");
    dwHardware->setIcon("EntityIcons", "HardwareDefinitions");


    //Set Icon Visibility
    dwInterfaces->setIconVisible(false);
    dwBehaviour->setIconVisible(false);
    dwAssemblies->setIconVisible(false);
    dwHardware->setIconVisible(false);

    //Protected from deletion
    dwInterfaces->setProtected(true);
    dwBehaviour->setProtected(true);
    dwAssemblies->setProtected(true);
    dwHardware->setProtected(true);

    SettingsController* s = SettingsController::settings();

    //Set initial area
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dwInterfaces);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dwBehaviour);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dwAssemblies);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dwHardware);

    // NOTE: Apparently calling innerWindow's createPopupMenu crashes the
    // application if it's called before the dock widgets are added above
    // This function needs to be called after the code above
    setupMenuCornerWidget();
    setupDockablePanels();

    // resizeDocks({blueWidget, yellowWidget}, {20 , 40}, Qt::Horizontal);
    //innerWindow->resizeDocks({dwInterfaces, dwBehaviour, searchDockWidget}, {50, 50, 50}, Qt::Horizontal);
    //innerWindow->resizeDocks({dwInterfaces, dwAssemblies}, {50, 50}, Qt::Vertical);
}


/**
 * @brief MedeaMainWindow::setupWelcomeScreen
 */
void MainWindow::setupWelcomeScreen()
{
    welcomeScreen = new WelcomeScreenWidget(viewController->getActionController(), this);
    welcomeScreenOn = false;

    QWidget* holderWidget = new QWidget(this);
    holderWidget->hide();

    holderLayout = new QVBoxLayout(holderWidget);
    holderLayout->addWidget(welcomeScreen);

    connect(this, &MainWindow::recentProjectsUpdated, welcomeScreen, &WelcomeScreenWidget::recentProjectsUpdated);
}


/**
 * @brief MedeaMainWindow::setupMenuBar
 */
void MainWindow::setupMenuBar()
{
    menuBar = new QMenuBar(this);
    menuBar->setFixedHeight(TOOLBAR_HEIGHT);
    menuBar->setNativeMenuBar(false);
    setMenuBar(menuBar);

    ActionController* ac = viewController->getActionController();

    menuBar->addMenu(ac->menu_file);
    menuBar->addMenu(ac->menu_edit);
    menuBar->addMenu(ac->menu_view);
    menuBar->addMenu(ac->menu_model);
    menuBar->addMenu(ac->menu_jenkins);
    menuBar->addMenu(ac->menu_options);
    menuBar->addMenu(ac->menu_help);

    // moved the connection here otherwise this action won't be connected unless there's a notification toolbar
    //QAction* showNotificationPanel = ac->window_showNotifications;
    //connect(showNotificationPanel, &QAction::triggered, this, &MainWindow::ensureNotificationPanelVisible);
}


void MainWindow::toolbarOrientationChanged(Qt::Orientation orientation){
    if(applicationToolbar_spacer1 && applicationToolbar_spacer2){
        QSizePolicy::Policy  h_pol = QSizePolicy::Fixed;
        QSizePolicy::Policy  v_pol = QSizePolicy::Fixed;

        switch(orientation){
            case Qt::Horizontal:
                h_pol = QSizePolicy::Expanding;
                break;
            case Qt::Vertical:
                v_pol = QSizePolicy::Expanding;
                break;
        }

        applicationToolbar_spacer1->setSizePolicy(h_pol, v_pol);
        applicationToolbar_spacer2->setSizePolicy(h_pol, v_pol);
    }
}
/**
 * @brief MedeaMainWindow::setupToolBar
 */
void MainWindow::setupToolBar()
{
    applicationToolbar = new QToolBar("Toolbar", this);
    applicationToolbar->setObjectName("APPLICATION_TOOLBAR");
    applicationToolbar->setMovable(true);
    applicationToolbar->setFloatable(true);
    applicationToolbar->setIconSize(QSize(16,16));
    
    applicationToolbar_spacer1 = new QWidget(this);
    applicationToolbar_spacer2 = new QWidget(this);
    applicationToolbar->addWidget(applicationToolbar_spacer1);
    applicationToolbar->addActions(viewController->getActionController()->applicationToolbar->actions());
    applicationToolbar->addWidget(applicationToolbar_spacer2);

    
    connect(applicationToolbar, &QToolBar::orientationChanged, this, &MainWindow::toolbarOrientationChanged);
    innerWindow->addToolBar(applicationToolbar);
    toolbarOrientationChanged(Qt::Horizontal);
}


/**
 * @brief MedeaMainWindow::setupSearchBar
 */
void MainWindow::setupSearchBar()
{
    searchButton = new QToolButton(this);
    searchButton->setToolTip("Submit Search");

    searchCompleterModel = new QStringListModel(this);

    searchCompleter = new QCompleter(this);
    searchCompleter->setModel(searchCompleterModel);
    searchCompleter->setFilterMode(Qt::MatchContains);
    searchCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    searchCompleter->popup()->setItemDelegate(new QStyledItemDelegate(this));
    searchCompleter->popup()->setFont(QFont(font().family(), 10));

    searchBar = new QLineEdit(this);
    searchBar->setFont(QFont(font().family(), 13));
    searchBar->setPlaceholderText("Search Here...");
    searchBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    searchBar->setCompleter(searchCompleter);

    searchToolbar = new QToolBar(this);
    searchToolbar->setIconSize(QSize(24,24));
    searchToolbar->setMovable(false);
    searchToolbar->setFloatable(false);
    searchToolbar->addWidget(searchBar);
    searchToolbar->addWidget(searchButton);

    searchPopup = new PopupWidget(PopupWidget::POPUP, this);
    searchPopup->setWidget(searchToolbar);
    searchPopup->setWidth(300);

    connect(this, &MainWindow::requestSuggestions, viewController, &ViewController::requestSearchSuggestions);
    connect(viewController, &ViewController::vc_gotSearchSuggestions, this, &MainWindow::updateSearchSuggestions);

    connect(searchBar, SIGNAL(returnPressed()), searchButton, SLOT(click()));
    connect(searchButton, SIGNAL(clicked(bool)), searchPopup, SLOT(hide()));
    connect(searchButton, SIGNAL(clicked(bool)), this, SLOT(searchEntered()));
}


/**
 * @brief MedeaMainWindow::setupProgressBar
 */
void MainWindow::setupProgressBar()
{
    progressLabel = new QLabel("", this);
    progressLabel->setFont(QFont(font().family(), 11));
    progressLabel->setFixedHeight(progressLabel->sizeHint().height());
    progressLabel->setAlignment(Qt::AlignCenter);
    progressLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0,100);
    progressBar->setTextVisible(false);

    QWidget* widget = new QWidget(this);
    widget->setStyleSheet("QWidget{ background: rgba(0,0,0,0); border: 0px; }");

    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setSpacing(5);
    layout->setMargin(2);
    layout->addWidget(progressLabel);
    layout->addWidget(progressBar);

    progressPopup = new PopupWidget(PopupWidget::DIALOG, this);
    progressPopup->setWidget(widget);
    progressPopup->setWidth(widget->sizeHint().width() + 200);
    progressPopup->setHeight(progressLabel->sizeHint().height() + 30);
    progressPopup->hide();

    connect(viewController, &ViewController::mc_showProgress, this, &MainWindow::showProgressBar);
    connect(viewController, &ViewController::mc_progressChanged, this, &MainWindow::updateProgressBar);
}


/**
 * @brief MedeaMainWindow::setupNotificationBar
 */
void MainWindow::setupNotificationBar()
{
    notificationIconLabel = new QLabel(this);
    notificationIconLabel->setAlignment(Qt::AlignCenter);

    notificationLabel = new QLabel("This is a notification.", this);
    notificationLabel->setFont(QFont(font().family(), 11));
    notificationLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    //notificationLabel->setWordWrap(true);

    notificationWidget = new QWidget(this);
    notificationWidget->setContentsMargins(5, 2, 5, 2);
    notificationWidget->setStyleSheet("background: rgba(0,0,0,0); border: 0px;");

    QHBoxLayout* layout = new QHBoxLayout(notificationWidget);
    layout->setMargin(0);
    layout->setSpacing(5);
    layout->addWidget(notificationIconLabel, 0, Qt::AlignCenter);
    layout->addWidget(notificationLabel, 1, Qt::AlignCenter);

    notificationPopup = new PopupWidget(PopupWidget::POPUP, 0);
    notificationPopup->setWidget(notificationWidget);
    notificationPopup->hide();
    connect(NotificationManager::manager(), &NotificationManager::notificationAdded, this, &MainWindow::popupLatestNotification);

    notificationTimer = new QTimer(this);
    connect(notificationTimer, &QTimer::timeout, notificationPopup, &QDialog::hide);
}


/**
 * @brief MedeaMainWindow::setupDock
 */
void MainWindow::setupDock()
{
    dockTabWidget = new DockTabWidget(viewController, this);
    dockTabWidget->setMinimumWidth(150);
    dockTabWidget->setMaximumWidth(150);


    tool_Dock = WindowManager::constructToolDockWidget("Dock");
    tool_Dock->setWidget(dockTabWidget);
    tool_Dock->setAllowedAreas(Qt::LeftDockWidgetArea);

    connect(viewController->getActionController()->toggleDock, SIGNAL(triggered(bool)), tool_Dock, SLOT(setVisible(bool)));

    //Check visibility state.
    addDockWidget(Qt::LeftDockWidgetArea, tool_Dock, Qt::Vertical);
}


/**
 * @brief MedeaMainWindow::setupDataTable
 */
void MainWindow::setupDataTable()
{
    tool_Table = WindowManager::constructToolDockWidget("Table");
    tableWidget = new DataTableWidget(viewController, tool_Table);
    tableWidget->setMinimumHeight(200);
    tableWidget->setMinimumWidth(200);
    tool_Table->setWidget(tableWidget);
    tool_Table->setAllowedAreas(Qt::RightDockWidgetArea);

    //Add the rename action to the dockwidget so that it'll handle rename shortcut
    tool_Table->addAction(viewController->getActionController()->edit_renameActiveSelection);

    QAction* modelAction = viewController->getActionController()->model_selectModel;
    modelAction->setToolTip("Show Model's Table");
    tool_Table->getTitleBar()->addToolAction(modelAction, Qt::AlignLeft);

    //Check visibility state.
    addDockWidget(Qt::RightDockWidgetArea, tool_Table, Qt::Vertical);
}


/**
 * @brief MedeaMainWindow::setupMinimap
 */
void MainWindow::setupMinimap()
{
    minimap = new NodeViewMinimap(this);
    minimap->setMinimumHeight(100);
    minimap->setMinimumWidth(200);
    minimap->setMaximumWidth(200);
    

    tool_Minimap = WindowManager::constructToolDockWidget("Minimap");
    tool_Minimap->setWidget(minimap);
    tool_Minimap->setAllowedAreas(Qt::RightDockWidgetArea);

    addDockWidget(Qt::RightDockWidgetArea, tool_Minimap, Qt::Vertical);
}


/**
 * @brief MedeaMainWindow::setupMenuCornerWidget
 * NOTE: This neeeds to be called after the tool dock widgets
 * and both the central and inner windows are constructed.
 */
void MainWindow::setupMenuCornerWidget()
{
    QMenu* menu = QMainWindow::createPopupMenu();
    //Add a hide/show for toolbar
    menu->addAction(applicationToolbar->toggleViewAction());

    restoreToolsAction = menu->addAction("Reset All Tools");
    connect(restoreToolsAction, &QAction::triggered, this, &MainWindow::resetToolDockWidgets);

    restoreToolsButton = new QToolButton(this);
    restoreToolsButton->setToolTip("Restore Tool Dock Widgets");
    restoreToolsButton->setMenu(menu);
    restoreToolsButton->setPopupMode(QToolButton::InstantPopup);
    restoreToolsButton->setStyleSheet("QToolButton{ border-radius: 4px; }"
                                      "QToolButton::menu-indicator{ image: none; }");

    //notificationToolbar = new NotificationToolbar(viewController, this);
    notificationToolbar = NotificationManager::constructToolbar();
    notificationToolbar->setParent(this);
    connect(notificationToolbar, &NotificationToolbar::toggleDialog, this, &MainWindow::toggleNotificationPanel);
    connect(notificationToolbar, &NotificationToolbar::showLastNotification, this, &MainWindow::popupLatestNotification);
    
    connect(NotificationManager::manager(), &NotificationManager::updateNotificationToolbarSize, this, &MainWindow::updateMenuBarSize);

    QToolBar* tb = new QToolBar(this);
    tb->setStyleSheet("QToolBar{ padding: 0px; } QToolButton{ padding: 3px 2px; }");
    tb->addWidget(restoreToolsButton);

    QWidget* w = new QWidget(this);
    w->setFixedHeight(menuBar->height() - 6);
    menuBar->setCornerWidget(w);

    QHBoxLayout* hLayout = new QHBoxLayout(w);
    hLayout->setMargin(0);
    hLayout->addWidget(notificationToolbar);
    hLayout->addSpacerItem(new QSpacerItem(3,0));
    hLayout->addWidget(tb);
}


/**
 * @brief MainWindow::setupDockablePanels
 * This sets up the search and notification dialogs.
 */
void MainWindow::setupDockablePanels()
{
    auto dwQOSBrowser = WindowManager::constructViewDockWidget("QOS Browser");
    dwQOSBrowser->setWidget(new QOSBrowser(viewController, dwQOSBrowser));
    //dwQOSBrowser->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dwQOSBrowser->setIconVisible(true);
    dwQOSBrowser->setIcon("EntityIcons", "QOSProfile");
    dwQOSBrowser->setProtected(true);

    searchPanel = new SearchDialog(this);
    searchDockWidget = WindowManager::constructViewDockWidget("Search Results");
    searchDockWidget->setWidget(searchPanel);
    searchDockWidget->setIcon("Icons", "zoomInPage");
    searchDockWidget->setIconVisible(true);
    searchDockWidget->setProtected(true);

    notificationPanel = NotificationManager::constructPanel();
    notificationPanel->setParent(this);
    connect(NotificationManager::manager(), &NotificationManager::showNotificationPanel, this, &MainWindow::ensureNotificationPanelVisible);

    notificationDockWidget = WindowManager::constructViewDockWidget("Notifications");
    notificationDockWidget->setWidget(notificationPanel);
    notificationDockWidget->setIcon("Icons", "exclamationInBubble");
    notificationDockWidget->setIconVisible(true);
    notificationDockWidget->setProtected(true);

    jenkinsDockWidget = WindowManager::constructViewDockWidget("Jenkins");
    jenkinsDockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    jenkinsDockWidget->setIcon("Icons", "jenkinsFlat");
    jenkinsDockWidget->setIconVisible(true);
    jenkinsDockWidget->setProtected(true);

    //dwQOSBrowser->widget()->setMinimumSize(searchDockWidget->widget()->minimumSize());

    // add tool dock widgets to the inner window
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dwQOSBrowser);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, searchDockWidget);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, notificationDockWidget);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, jenkinsDockWidget);

    // initially hide tool dock widgets
    innerWindow->setDockWidgetVisibility(searchDockWidget, false);
    innerWindow->setDockWidgetVisibility(notificationDockWidget, false);
    innerWindow->setDockWidgetVisibility(jenkinsDockWidget, false);
    innerWindow->tabifyDockWidget(searchDockWidget, notificationDockWidget);

    /**
     * Initially have the search and notification panels and the QoS browser in a separate window
     * NOTE: Need to add the dock widget to the inner window first before reparenting it so that it's protected from deletion
     */
    /*
    BaseWindow* bw = WindowManager::constructSubWindow("Tool Dock Widgets");
    bw->setWindowTitle("Sub Window #" + QString::number(bw->getID() - 2));

    // remove tool dock widgets from the inner window
    innerWindow->removeDockWidget(dwQOSBrowser);
    innerWindow->removeDockWidget(searchDockWidget);
    innerWindow->removeDockWidget(notificationDockWidget);

    // reparent dock widgets
    bw->addDockWidget(Qt::LeftDockWidgetArea, searchDockWidget);
    bw->addDockWidget(Qt::LeftDockWidgetArea, notificationDockWidget);
    bw->tabifyDockWidget(searchDockWidget, notificationDockWidget);
    bw->addDockWidget(Qt::RightDockWidgetArea, dwQOSBrowser);

    bw->setDockWidgetVisibility(dwQOSBrowser, SettingsController::settings()->getSetting(SETTINGS::WINDOW_QOS_VISIBLE).toBool());
    bw->setDockWidgetVisibility(searchDockWidget, true);
    bw->setDockWidgetVisibility(notificationDockWidget, true);

    bw->show();
    */

    if (viewController) {
        connect(viewController, &ViewController::vc_SetupModelController, searchPanel, &SearchDialog::resetPanel);
        connect(searchPanel, SIGNAL(centerOnViewItem(int)), viewController, SLOT(centerOnID(int)));
        connect(searchPanel, SIGNAL(popupViewItem(int)), viewController, SLOT(popupItem(int)));
        connect(searchPanel, SIGNAL(itemHoverEnter(int)), viewController->getToolbarController(), SLOT(actionHoverEnter(int)));
        connect(searchPanel, SIGNAL(itemHoverLeave(int)), viewController->getToolbarController(), SLOT(actionHoverLeave(int)));
        connect(notificationPanel, &NotificationDialog::centerOn, viewController, &ViewController::centerOnID);
        connect(notificationPanel, &NotificationDialog::popup, viewController, &ViewController::popupItem);
        connect(notificationPanel, SIGNAL(itemHoverEnter(int)), viewController->getToolbarController(), SLOT(actionHoverEnter(int)));
        connect(notificationPanel, SIGNAL(itemHoverLeave(int)), viewController->getToolbarController(), SLOT(actionHoverLeave(int)));
    }
    connect(searchPanel, &SearchDialog::searchButtonClicked, this, &MainWindow::popupSearch);
    connect(searchPanel, &SearchDialog::refreshButtonClicked, this, &MainWindow::searchEntered);
}


/**
 * @brief MedeaMainWindow::setupViewManager
 */
void MainWindow::setupViewManager()
{
    viewManager = WindowManager::manager()->getViewManagerGUI();
    viewManager->setMinimumHeight(100);
    viewManager->setMinimumWidth(200);

    tool_ViewManager = WindowManager::constructToolDockWidget("View Manager");
    tool_ViewManager->setWidget(viewManager);
    tool_ViewManager->setAllowedAreas(Qt::RightDockWidgetArea);

    addDockWidget(Qt::RightDockWidgetArea, tool_ViewManager, Qt::Vertical);
}


/**
 * @brief MedeaMainWindow::setupJenkinsManager
 */
void MainWindow::setupJenkinsManager()
{
    auto jenkinsManager = viewController->getJenkinsManager();
    auto jmw = jenkinsManager->GetJobMonitorWidget();
    jenkinsDockWidget->setWidget(jmw);
    connect(viewController, &ViewController::vc_executeJenkinsJob, this, [this](QString){jenkinsDockWidget->setVisible(true);});
}



/**
 * @brief MedeaMainWindow::resizeToolWidgets
 */
void MainWindow::resizeToolWidgets()
{
    //Reset the RIght Panel
    resizeDocks({tool_Table, tool_ViewManager, tool_Minimap}, {1,2,1}, Qt::Vertical);
    resizeDocks({tool_Table, tool_ViewManager, tool_Minimap, tool_Dock}, {1,1,1, 1}, Qt::Vertical);
}


/**
 * @brief MedeaMainWindow::moveWidget
 * @param widget
 * @param parentWidget
 * @param alignment
 */
void MainWindow::moveWidget(QWidget* widget, QWidget* parentWidget, Qt::Alignment alignment)
{
    QWidget* cw = parentWidget;
    QPointF widgetPos;
    if (cw == 0) {
        //Check for active Window
        cw = QApplication::activeWindow();
        //Check
        if (!cw || !cw->isWindowType()) {
            cw = WindowManager::manager()->getActiveWindow();
            //cw = QApplication::activeWindow();
            //qDebug() << "NOT A WINDOW - " << cw;
        } else {
            //qDebug() << "WINDOW";
        }
    }
    if (cw == innerWindow) {
        widgetPos = pos() - QPointF(0,8);
    } else {
        widgetPos.ry() -= widget->height()/2 + 8;
    }
    if (cw && widget) {
        widgetPos += cw->geometry().center();
        switch (alignment) {
        case Qt::AlignBottom:
            widgetPos.ry() += cw->height()/2;
            break;
        default:
            break;
        }
        widgetPos -= QPointF(widget->width()/2, widget->height()/2);
        widget->move(widgetPos.x(), widgetPos.y());
        //cw->activateWindow();
    }
}


/**
 * @brief MedeaMainWindow::resizeEvent
 * @param e
 */
void MainWindow::resizeEvent(QResizeEvent* e)
{
    if(e){
        QMainWindow::resizeEvent(e);
    }
    if(applicationToolbar && applicationToolbar->orientation() == Qt::Vertical){
        applicationToolbar->setFixedHeight(centralWidget()->rect().height());
    }
}


/**
 * @brief MedeaMainWindow::closeEvent
 * @param event
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    //Save the state
    if(!welcomeScreenOn){
        saveWindowState();
    }
    if (viewController) {
        viewController->closeMEDEA();
        event->ignore();
    }
}
