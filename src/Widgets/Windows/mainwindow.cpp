#include "mainwindow.h"
#include "../DockWidgets/viewdockwidget.h"
#include "../DockWidgets/tooldockwidget.h"


#include "../../Controllers/SelectionController/selectioncontroller.h"
#include "../../Controllers/SettingsController/settingscontroller.h"

#include "../../Widgets/ViewManager/viewmanagerwidget.h"
#include "../../Widgets/Jenkins/jenkinsjobmonitorwidget.h"
#include "../../Widgets/optiongroupbox.h"
#include "../../Controllers/NotificationManager/notificationobject.h"
#include "../../theme.h"

#include "../../Utils/filtergroup.h"
#include "../../Widgets/customgroupbox.h"
#include "../../Widgets/Dialogs/progresspopup.h"
#include "../../Controllers/SearchManager/searchmanager.h"

#include <QDebug>
#include <QHeaderView>
#include <QPushButton>
#include <QMenuBar>
#include <QDateTime>
#include <QApplication>
#include <QStringBuilder>
#include <QStringListModel>
#include <QTabWidget>
#include <QDesktopWidget>

#define TOOLBAR_HEIGHT 32
#define MENU_ICON_SIZE 16


/**
 * @brief MedeaMainWindow::MedeaMainWindow
 * @param vc
 * @param parent
 */
MainWindow::MainWindow(ViewController *vc, QWidget* parent):BaseWindow(parent, BaseWindow::MAIN_WINDOW)
{
    setDockNestingEnabled(true);
    setDockOptions(QMainWindow::AnimatedDocks);

    viewController = vc;
    initializeApplication();
    setContextMenuPolicy(Qt::NoContextMenu);
    setupTools();
    setupInnerWindow();
    

    setViewController(vc);
    setupJenkinsManager();


   

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(WindowManager::manager(), SIGNAL(activeViewDockWidgetChanged(ViewDockWidget*,ViewDockWidget*)), this, SLOT(activeViewDockWidgetChanged(ViewDockWidget*, ViewDockWidget*)));
    connect(this, &MainWindow::welcomeScreenToggled, viewController, &ViewController::welcomeScreenToggled);

    SettingsController* s = SettingsController::settings();

    auto outer_geo = s->getSetting(SETTINGS::WINDOW_OUTER_GEOMETRY).toByteArray();
    if(!outer_geo.isEmpty()){
        restoreGeometry(outer_geo);
    }else{
        
        
    }

    setModelTitle();
    themeChanged();
    toggleWelcomeScreen(true);
    show();
    
    /*
    auto group_box = new OptionGroupBox("Test group", this);
    for(auto s : getNotificationSeverities()){
        group_box->addOption(static_cast<uint>(s), Notification::getSeverityString(s), "Icons", getSeverityIcon(s));
    }
    group_box->addOption("HELLO", "HELLO", "Icons", "circle");
    auto vals = group_box->getOptions<Notification::Severity>();
    for (auto v : vals){
        qCritical() << Notification::getSeverityString(v);
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

    connect(viewController, &ViewController::mc_projectModified, this, &MainWindow::setWindowModified);
    connect(viewController, &ViewController::vc_projectPathChanged, this, &MainWindow::setModelTitle);
    connect(viewController, &ViewController::vc_showWelcomeScreen, this, &MainWindow::toggleWelcomeScreen);

    connect(controller, &SelectionController::itemActiveSelectionChanged, tableWidget, &DataTableWidget::itemActiveSelectionChanged);

    if (actionController) {
        connect(actionController->edit_search, &QAction::triggered, SearchManager::manager(), &SearchManager::PopupSearch);
    }

    connect(SearchManager::manager(), &SearchManager::SearchComplete, this, &MainWindow::showSearchDialog);
    connect(NotificationManager::manager(), &NotificationManager::showNotificationPanel, this, &MainWindow::showNotificationDialog);
}

void MainWindow::showSearchDialog()
{
    WindowManager::manager()->showDockWidget(dockwidget_Search);
}


/**
 * @brief MainWindow::toggleNotificationPanel
 * This toggles the visibility of the notification panel dock widget.
 * If it's already visible but its parent window is not active, activate
 * the window instead of hiding it. Otherwise, hide the dock widget.
 */
void MainWindow::showNotificationDialog()
{
    WindowManager::manager()->showDockWidget(dockwidget_Notification);
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

    QString menuStyle = theme->getMenuStyleSheet(MENU_ICON_SIZE);
    ActionController* actionController = viewController->getActionController();

    actionController->menu_file->setStyleSheet(menuStyle);
    actionController->menu_file_recentProjects->setStyleSheet(menuStyle);
    actionController->menu_edit->setStyleSheet(menuStyle);
    actionController->menu_view->setStyleSheet(menuStyle);
    actionController->menu_model->setStyleSheet(menuStyle);
    actionController->menu_jenkins->setStyleSheet(menuStyle);
    actionController->menu_help->setStyleSheet(menuStyle);
    actionController->menu_options->setStyleSheet(menuStyle);

    applicationToolbar->toggleViewAction()->setIcon(theme->getIcon("WindowIcon", applicationToolbar->windowTitle()));

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
    if(prevDock){
        auto prev_node_view = prevDock->getNodeView();
        
        if(prev_node_view){
            minimap->disconnect(prev_node_view);
            prev_node_view->disconnect(minimap);
        }
    }

    NodeView* node_view = 0;

    //Attach new view
    if(viewDock){
        node_view = viewDock->getNodeView();
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
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    //Start MEDEA centralized and 1200x800
    resize(1200, 800);
    auto default_screen_geo = QApplication::desktop()->screenGeometry();
    auto pos_x = (default_screen_geo.width() - width()) / 2;
    auto pos_y = (default_screen_geo.height() - height()) / 2;
    move(pos_x, pos_y);
}

void MainWindow::swapCentralWidget(QWidget* widget){
    if(centralWidget()){
        //Setting the parent of the centralWidget will stop
        //the QMainWindow deleting the old widget when a new central widget is deleted
        centralWidget()->setParent(0);
    }
    setCentralWidget(widget);
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
    if(on){
        swapCentralWidget(welcomeScreen);
    }else{
        swapCentralWidget(innerWindow);
        restoreWindowState(false);
        //Call this after everything has loaded
        //NotificationManager::manager()->popupLatestNotification();
        //QMetaObject::invokeMethod(NotificationManager::manager(), "popupLatestNotification", Qt::QueuedConnection);
    }
    emit welcomeScreenToggled(on);
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

void MainWindow::restoreWindowState(bool restore_geo){
    SettingsController* s = SettingsController::settings();
    if(s){
        auto load_dock = s->getSetting(SETTINGS::GENERAL_SAVE_DOCKS_ON_EXIT).toBool();
        auto load_window = s->getSetting(SETTINGS::GENERAL_SAVE_WINDOW_ON_EXIT).toBool();

        auto outer_state = s->getSetting(SETTINGS::WINDOW_OUTER_STATE).toByteArray();
        auto inner_state = s->getSetting(SETTINGS::WINDOW_INNER_STATE).toByteArray();

        //Check if any are invalid
        bool invalid = outer_state.isEmpty() || inner_state.isEmpty();

        if(invalid){
            resetToolDockWidgets();
        }else{
            if(load_window){
                restoreState(outer_state);
            }

            if(load_dock){
                innerWindow->restoreState(inner_state);
            }
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


    restoreToolsButton = 0;
    restoreToolsAction = 0;

    viewManager = 0;

    setupWelcomeScreen();
    setupMenuBar();
    setupProgressBar();
    setupDock();
    
    setupDataTable();
    setupViewManager();
    setupMinimap();
}


/**
 * @brief MedeaMainWindow::setupInnerWindow
 */
void MainWindow::setupInnerWindow()
{   
    innerWindow = WindowManager::manager()->constructCentralWindow("Main Window");
    //Construct dockWidgets.
    auto dwInterfaces = viewController->constructViewDockWidget("Interfaces");
    auto dwBehaviour = viewController->constructViewDockWidget("Behaviour");
    auto dwAssemblies = viewController->constructViewDockWidget("Assemblies");
    auto dwHardware = viewController->constructViewDockWidget("Hardware");


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

    //Setip
    auto theme = Theme::theme();
    theme->setWindowIcon("InterfaceDefinitions", "EntityIcons", "InterfaceDefinitions");
    theme->setWindowIcon("BehaviourDefinitions", "EntityIcons", "BehaviourDefinitions");
    theme->setWindowIcon("AssemblyDefinitions", "EntityIcons", "AssemblyDefinitions");
    theme->setWindowIcon("HardwareDefinitions", "EntityIcons", "HardwareDefinitions");

    dwInterfaces->setIcon("WindowIcon", "InterfaceDefinitions");
    dwBehaviour->setIcon("WindowIcon", "BehaviourDefinitions");
    dwAssemblies->setIcon("WindowIcon", "AssemblyDefinitions");
    dwHardware->setIcon("WindowIcon", "HardwareDefinitions");


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

    setupToolBar();
    setupMenuCornerWidget();
    setupDockablePanels();    
}


/**
 * @brief MedeaMainWindow::setupWelcomeScreen
 */
void MainWindow::setupWelcomeScreen()
{
    welcomeScreen = new WelcomeScreenWidget(viewController->getActionController(), this);
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

    auto menu_style = new CustomMenuStyle(MENU_ICON_SIZE);

    for(auto action : menuBar->actions()){
        auto menu = action->menu();
        if(menu){
            menu->setStyle(menu_style);
        }
    }

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
    Theme::theme()->setWindowIcon(applicationToolbar->windowTitle(), "Icons", "spanner");

    
    
    applicationToolbar_spacer1 = new QWidget(this);
    applicationToolbar_spacer2 = new QWidget(this);
    applicationToolbar->addWidget(applicationToolbar_spacer1);
    applicationToolbar->addActions(viewController->getActionController()->applicationToolbar->actions());
    applicationToolbar->addWidget(applicationToolbar_spacer2);

    
    connect(applicationToolbar, &QToolBar::orientationChanged, this, &MainWindow::toolbarOrientationChanged);
    toolbarOrientationChanged(Qt::Horizontal);
    innerWindow->addToolBar(applicationToolbar);
}   



/**
 * @brief MedeaMainWindow::setupProgressBar
 */
void MainWindow::setupProgressBar()
{
    auto popup = new ProgressPopup();
    connect(viewController, &ViewController::mc_showProgress, popup, &ProgressPopup::ProgressUpdated);
    connect(viewController, &ViewController::mc_progressChanged, popup, &ProgressPopup::UpdateProgressBar);
}


/**
 * @brief MedeaMainWindow::setupDock
 */
void MainWindow::setupDock()
{
    dockTabWidget = new DockTabWidget(viewController, this);
    dockTabWidget->setMinimumWidth(180);
    //dockTabWidget->setFixedWidth(180);

    dockwidget_Dock = WindowManager::manager()->constructToolDockWidget("Dock");
    dockwidget_Dock->setWidget(dockTabWidget);
    dockwidget_Dock->setAllowedAreas(Qt::LeftDockWidgetArea);

    connect(viewController->getActionController()->toggleDock, SIGNAL(triggered(bool)), dockwidget_Dock, SLOT(setVisible(bool)));

    Theme::theme()->setWindowIcon(dockwidget_Dock->getTitle(), "Icons", "circleCirclesDark");
    dockwidget_Dock->setIcon("WindowIcon", dockwidget_Dock->getTitle());

    //Check visibility state.
    addDockWidget(Qt::LeftDockWidgetArea, dockwidget_Dock, Qt::Vertical);
}


/**
 * @brief MedeaMainWindow::setupDataTable
 */
void MainWindow::setupDataTable()
{
    dockwidget_Table = WindowManager::manager()->constructToolDockWidget("Data Table");
    tableWidget = new DataTableWidget(viewController, dockwidget_Table);
    tableWidget->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred));
    tableWidget->setMinimumHeight(200);
    tableWidget->setMinimumWidth(200);
    dockwidget_Table->setWidget(tableWidget);
    dockwidget_Table->setAllowedAreas(Qt::RightDockWidgetArea);

    
    Theme::theme()->setWindowIcon(dockwidget_Table->getTitle(), "Icons", "sliders");
    dockwidget_Table->setIcon("WindowIcon", dockwidget_Table->getTitle());

    //Add the rename action to the dockwidget so that it'll handle rename shortcut
    dockwidget_Table->addAction(viewController->getActionController()->edit_renameActiveSelection);

    QAction* modelAction = viewController->getActionController()->model_selectModel;
    modelAction->setToolTip("Show Model's Table");
    dockwidget_Table->getTitleBar()->addToolAction(modelAction, Qt::AlignLeft);

    //Check visibility state.
    addDockWidget(Qt::RightDockWidgetArea, dockwidget_Table, Qt::Vertical);
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
    
    
    dockwidget_Minimap = WindowManager::manager()->constructToolDockWidget("Minimap");
    dockwidget_Minimap->setWidget(minimap);
    dockwidget_Minimap->setAllowedAreas(Qt::RightDockWidgetArea);

    Theme::theme()->setWindowIcon(dockwidget_Minimap->getTitle(), "Icons", "map");
    dockwidget_Minimap->setIcon("WindowIcon", dockwidget_Minimap->getTitle());

    addDockWidget(Qt::RightDockWidgetArea, dockwidget_Minimap, Qt::Vertical);
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
    notificationToolbar = NotificationManager::manager()->getToolbar();
    notificationToolbar->setParent(this); 

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
    auto manager = WindowManager::manager();
    auto dwQOSBrowser = manager->constructDockWidget("QOS Browser");
    dwQOSBrowser->setWidget(new QOSBrowser(viewController, dwQOSBrowser));
    dwQOSBrowser->setIconVisible(true);
    
    auto theme = Theme::theme();
    theme->setWindowIcon("QOS Browser", "Icons", "speedGauge");
    theme->setWindowIcon("Search Results", "Icons", "zoomInPage");
    theme->setWindowIcon("Notifications", "Icons", "exclamationInBubble");
    theme->setWindowIcon("Jenkins", "Icons", "jenkinsFlat");

    dwQOSBrowser->setIcon("WindowIcon", "QOS Browser");
    dwQOSBrowser->setProtected(true);

    auto searchPanel = SearchManager::manager()->getSearchDialog();
    
    dockwidget_Search = manager->constructDockWidget("Search Results");
    dockwidget_Search->setWidget(searchPanel);
    dockwidget_Search->setIcon("WindowIcon", "Search Results");
    dockwidget_Search->setIconVisible(true);
    dockwidget_Search->setProtected(true);
    
    auto notificationPanel = NotificationManager::manager()->getPanel();
    
    

    dockwidget_Notification = manager->constructDockWidget("Notifications");
    dockwidget_Notification->setWidget(notificationPanel);
    dockwidget_Notification->setIcon("WindowIcon", "Notifications");
    dockwidget_Notification->setIconVisible(true);
    dockwidget_Notification->setProtected(true);

    dockwidget_Jenkins = manager->constructDockWidget("Jenkins");
    dockwidget_Jenkins->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dockwidget_Jenkins->setIcon("WindowIcon", "Jenkins");
    dockwidget_Jenkins->setIconVisible(true);
    dockwidget_Jenkins->setProtected(true);


    // add tool dock widgets to the inner window
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dwQOSBrowser);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dockwidget_Search);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dockwidget_Notification);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dockwidget_Jenkins);

    // initially hide tool dock widgets
    innerWindow->setDockWidgetVisibility(dwQOSBrowser, false);
    innerWindow->setDockWidgetVisibility(dockwidget_Search, false);
    innerWindow->setDockWidgetVisibility(dockwidget_Notification, false);
    innerWindow->setDockWidgetVisibility(dockwidget_Jenkins, false);
    innerWindow->tabifyDockWidget(dockwidget_Search, dockwidget_Notification);
}


/**
 * @brief MedeaMainWindow::setupViewManager
 */
void MainWindow::setupViewManager()
{
    auto manager = WindowManager::manager();
    viewManager = manager->getViewManagerGUI();
    viewManager->setMinimumHeight(100);
    viewManager->setMinimumWidth(200);

    dockwidget_ViewManager = manager->constructToolDockWidget("View Manager");
    dockwidget_ViewManager->setWidget(viewManager);
    dockwidget_ViewManager->setAllowedAreas(Qt::RightDockWidgetArea);

    Theme::theme()->setWindowIcon(dockwidget_ViewManager->getTitle(), "Icons", "gridCombo");
    dockwidget_ViewManager->setIcon("WindowIcon", dockwidget_ViewManager->getTitle());


    addDockWidget(Qt::RightDockWidgetArea, dockwidget_ViewManager, Qt::Vertical);
}


/**
 * @brief MedeaMainWindow::setupJenkinsManager
 */
void MainWindow::setupJenkinsManager()
{
    auto jenkinsManager = viewController->getJenkinsManager();
    auto jmw = jenkinsManager->GetJobMonitorWidget();
    dockwidget_Jenkins->setWidget(jmw);
    connect(viewController, &ViewController::vc_executeJenkinsJob, this, [this](QString){dockwidget_Jenkins->setVisible(true);});
}



/**
 * @brief MedeaMainWindow::resizeToolWidgets
 */
void MainWindow::resizeToolWidgets()
{
    //Reset the RIght Panel
    resizeDocks({dockwidget_Table, dockwidget_ViewManager, dockwidget_Minimap}, {1,2,1}, Qt::Vertical);
    resizeDocks({dockwidget_Dock, dockwidget_Table}, {1,1}, Qt::Horizontal);
}


/**
 * @brief MedeaMainWindow::moveWidget
 * @param widget
 * @param parentWidget
 * @param alignment
 */
void MainWindow::moveWidget(QWidget* widget, QWidget* parentWidget, Qt::Alignment alignment)
{
    auto center_widget = parentWidget;
    
    if(!center_widget){
        center_widget = QApplication::activeWindow();
        //Check
        if(!center_widget || !center_widget->isWindowType()){
            center_widget = WindowManager::manager()->getActiveWindow();
        }
    }

    if(widget && center_widget){
        auto pos = center_widget->mapToGlobal(center_widget->rect().center());
        auto widget_size = widget->frameGeometry();
        switch (alignment) {
        case Qt::AlignBottom:
            //Move to the bottom
            pos.ry() += center_widget->height() / 2;
            //Offset by the height of the widget
            pos.ry() -= widget_size.height();
            break;
        default:
            //Offset by the half the height of the widget
            pos.ry() -= widget_size.height() / 2;
            break;
        }
        //Center the widget
        pos.rx() -= widget_size.width() / 2;
        widget->move(pos);
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
    saveWindowState();
    if (viewController) {
        viewController->closeMEDEA();
        event->ignore();
    }
}
