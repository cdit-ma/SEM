#include "mainwindow.h"
#include "../DockWidgets/viewdockwidget.h"
#include "../DockWidgets/tooldockwidget.h"
#include "../DockWidgets/invisibledockwidget.h"

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
MainWindow::MainWindow(ViewController* view_controller, QWidget* parent):BaseWindow(parent, BaseWindow::MAIN_WINDOW)
{


    setViewController(view_controller);
    
    initializeApplication();
    setContextMenuPolicy(Qt::NoContextMenu);


    setupTools();
    setupInnerWindow();
    
    addDockWidget(Qt::BottomDockWidgetArea, dockwidget_Dock);
    addDockWidget(Qt::BottomDockWidgetArea, dockwidget_Center);
    addDockWidget(Qt::BottomDockWidgetArea, dockwidget_Right);
    
    
    
    setupDockIcons();

   
    connect(Theme::theme(), &Theme::theme_Changed, this, &MainWindow::themeChanged);
    connect(this, &MainWindow::welcomeScreenToggled, view_controller, &ViewController::welcomeScreenToggled);

    connect(action_controller->edit_search, &QAction::triggered, SearchManager::manager(), &SearchManager::PopupSearch);

    connect(SearchManager::manager(), &SearchManager::SearchComplete, this, [=](){WindowManager::manager()->showDockWidget(dockwidget_Search);});
    connect(NotificationManager::manager(), &NotificationManager::showNotificationPanel, this, [=](){WindowManager::manager()->showDockWidget(dockwidget_Notification);});
    connect(view_controller, &ViewController::vc_executeJenkinsJob, this, [=](){WindowManager::manager()->showDockWidget(dockwidget_Jenkins);});

    
    SettingsController* s = SettingsController::settings();

    auto outer_geo = s->getSetting(SETTINGS::WINDOW_OUTER_GEOMETRY).toByteArray();
    if(!outer_geo.isEmpty()){
        restoreGeometry(outer_geo);
    }

    setModelTitle();
    themeChanged();
    toggleWelcomeScreen(true);
    show();
}


/**
 * @brief MedeaMainWindow::setview_controller
 * @param vc
 */
void MainWindow::setViewController(ViewController* view_controller)
{
    this->view_controller = view_controller;
    action_controller = view_controller->getActionController();

    connect(view_controller, &ViewController::mc_projectModified, this, &MainWindow::setWindowModified);
    connect(view_controller, &ViewController::vc_projectPathChanged, this, &MainWindow::setModelTitle);
    connect(view_controller, &ViewController::vc_showWelcomeScreen, this, &MainWindow::toggleWelcomeScreen);
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

    menu_bar->setStyleSheet(theme->getMenuBarStyleSheet());


    QString menuStyle = theme->getMenuStyleSheet(MENU_ICON_SIZE);
    ActionController* actionController = view_controller->getActionController();

    actionController->menu_file->setStyleSheet(menuStyle);
    actionController->menu_file_recentProjects->setStyleSheet(menuStyle);
    actionController->menu_edit->setStyleSheet(menuStyle);
    actionController->menu_view->setStyleSheet(menuStyle);
    actionController->menu_model->setStyleSheet(menuStyle);
    actionController->menu_jenkins->setStyleSheet(menuStyle);
    actionController->menu_help->setStyleSheet(menuStyle);
    actionController->menu_options->setStyleSheet(menuStyle);

    
    applicationToolbar->toggleViewAction()->setIcon(theme->getIcon("WindowIcon", applicationToolbar->windowTitle()));

    restoreToolsAction->setIcon(theme->getIcon("Icons", "refresh"));

    
}

/**
 * @brief MedeaMainWindow::setModelTitle
 * @param modelTitle
 */
void MainWindow::setModelTitle(QString model_title)
{
    if(model_title.size()){
        model_title = "- " % model_title;
    }
    auto title = "MEDEA " % model_title % "[*]";
    setWindowTitle(title);
}


/**
 * @brief MedeaMainWindow::initializeApplication
 */
void MainWindow::initializeApplication()
{
    //Allow Drops
    setAcceptDrops(true);
    setDockNestingEnabled(true);
    setDockOptions(QMainWindow::AnimatedDocks);

    //Set QApplication information.
    QApplication::setApplicationName("MEDEA");
    QApplication::setApplicationVersion(APP_VERSION());
    QApplication::setOrganizationName("CDIT-MA");
    QApplication::setOrganizationDomain("https://github.com/cdit-ma/");
    QApplication::setWindowIcon(Theme::theme()->getIcon("Icons", "medeaLogo"));

   

    
    setCorner(Qt::TopLeftCorner, Qt::TopDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::TopDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

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
    auto welcome_screen_on = centralWidget() == welcomeScreen;

    if(welcome_screen_on != on){
        menu_bar->setVisible(!on);
        setDockWidgetsVisible(!on);

        //Swap between the welcome scree and 0
        swapCentralWidget(on ? welcomeScreen : 0);
        
        if(!on){
            restoreWindowState(false);
        }
        emit welcomeScreenToggled(on);
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

            s->setSetting(SETTINGS::WINDOW_RIGHT_GEOMETRY, rightWindow->saveGeometry());
            s->setSetting(SETTINGS::WINDOW_RIGHT_STATE, rightWindow->saveState());
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
        auto right_state = s->getSetting(SETTINGS::WINDOW_RIGHT_STATE).toByteArray();

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
                rightWindow->restoreState(right_state);
            }
        }
    }
}

void MainWindow::setDockWidgetIcon(BaseDockWidget* dock_widget, QString icon_path, QString icon_alias, Theme* theme){
    if(dock_widget){
        if(!theme){
            theme = Theme::theme();
        }
        auto title = dock_widget->getTitle();
        theme->setWindowIcon(title, icon_path, icon_alias);
        dock_widget->setIcon("WindowIcon", title);
    }
}

void MainWindow::setupDockIcons(){
    auto theme = Theme::theme();
    setDockWidgetIcon(dockwidget_Dock, "Icons", "circleCirclesDark", theme);
    setDockWidgetIcon(dockwidget_ViewManager, "Icons", "gridCombo", theme);
    
    setDockWidgetIcon(dockwidget_Table, "Icons", "sliders", theme);
    setDockWidgetIcon(dockwidget_Minimap, "Icons", "map", theme);
    setDockWidgetIcon(dockwidget_Qos, "Icons", "speedGauge", theme);
    setDockWidgetIcon(dockwidget_Search, "Icons", "zoomInPage", theme);
    setDockWidgetIcon(dockwidget_Notification, "Icons", "exclamationInBubble", theme);
    setDockWidgetIcon(dockwidget_Jenkins, "Icons", "jenkinsFlat", theme);
    setDockWidgetIcon(dockwidget_Dock, "Icons", "zoomInPage", theme);
    
}

/**
 * @brief MedeaMainWindow::setupTools
 */
void MainWindow::setupTools()
{
    //Setup Welcome screen
    welcomeScreen = new WelcomeScreenWidget(view_controller->getActionController(), this);
    
    //Setup Progress Bar
    auto progress_bar = new ProgressPopup();
    connect(view_controller, &ViewController::mc_showProgress, progress_bar, &ProgressPopup::ProgressUpdated);
    connect(view_controller, &ViewController::mc_progressChanged, progress_bar, &ProgressPopup::UpdateProgressBar);


   
    setupMenuBar();

    auto window_manager = WindowManager::manager();
    
    if(!dockwidget_Dock){
        dockwidget_Dock = window_manager->constructToolDockWidget("Dock");
        dockwidget_Dock->setWidget(new DockTabWidget(view_controller, this));
    }

    if(!dockwidget_Table){
        dockwidget_Table = window_manager->constructToolDockWidget("Data Table");
        dockwidget_Table->setWidget(new DataTableWidget(view_controller, dockwidget_Table));
    }

    if(!dockwidget_Minimap){
        dockwidget_Minimap = window_manager->constructToolDockWidget("Minimap");
        dockwidget_Minimap->setWidget(new NodeViewMinimap(this));
    }

    if(!dockwidget_ViewManager){
        dockwidget_ViewManager = window_manager->constructToolDockWidget("View Manager");
        dockwidget_ViewManager->setWidget(window_manager->getViewManagerGUI());
    }


    rightWindow = window_manager->constructSubWindow("Right Tools");
    rightWindow->setDockNestingEnabled(true);

    rightWindow->addDockWidget(Qt::TopDockWidgetArea, dockwidget_Table, Qt::Vertical);
    rightWindow->addDockWidget(Qt::TopDockWidgetArea, dockwidget_ViewManager, Qt::Vertical);
    rightWindow->addDockWidget(Qt::TopDockWidgetArea, dockwidget_Minimap, Qt::Vertical);

    dockwidget_Right = window_manager->constructInvisibleDockWidget("Right Tools");
    dockwidget_Right->setWidget(rightWindow);
}


/**
 * @brief MedeaMainWindow::setupInnerWindow
 */
void MainWindow::setupInnerWindow()
{   
    innerWindow = WindowManager::manager()->constructCentralWindow("Main Window");
    //Construct dockWidgets.
    auto dockwidget_Interfaces = view_controller->constructViewDockWidget("Interfaces");
    auto dockwidget_Behaviour = view_controller->constructViewDockWidget("Behaviour");
    auto dockwidget_Assemblies = view_controller->constructViewDockWidget("Assemblies");
    auto dockwidget_Hardware = view_controller->constructViewDockWidget("Hardware");


    //Set each NodeView with there contained aspects
    dockwidget_Interfaces->getNodeView()->setContainedViewAspect(VIEW_ASPECT::INTERFACES);
    dockwidget_Behaviour->getNodeView()->setContainedViewAspect(VIEW_ASPECT::BEHAVIOUR);
    dockwidget_Assemblies->getNodeView()->setContainedViewAspect(VIEW_ASPECT::ASSEMBLIES);
    dockwidget_Hardware->getNodeView()->setContainedViewAspect(VIEW_ASPECT::HARDWARE);

    //Setup Icons
    auto theme = Theme::theme();

    setDockWidgetIcon(dockwidget_Interfaces, "EntityIcons", "InterfaceDefinitions", theme);
    setDockWidgetIcon(dockwidget_Behaviour, "EntityIcons", "BehaviourDefinitions", theme);
    setDockWidgetIcon(dockwidget_Assemblies, "EntityIcons", "AssemblyDefinitions", theme);
    setDockWidgetIcon(dockwidget_Hardware, "EntityIcons", "HardwareDefinitions", theme);

    //Set Icon Visibility
    dockwidget_Interfaces->setIconVisible(false);
    dockwidget_Behaviour->setIconVisible(false);
    dockwidget_Assemblies->setIconVisible(false);
    dockwidget_Hardware->setIconVisible(false);

    //Protected from deletion
    dockwidget_Interfaces->setProtected(true);
    dockwidget_Behaviour->setProtected(true);
    dockwidget_Assemblies->setProtected(true);
    dockwidget_Hardware->setProtected(true);

    //Set initial area
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dockwidget_Interfaces);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dockwidget_Behaviour);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dockwidget_Assemblies);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dockwidget_Hardware);

    setupToolBar();
    setupMenuCornerWidget();
    setupDockablePanels();

    dockwidget_Center = WindowManager::manager()->constructInvisibleDockWidget("Central Widget");
    dockwidget_Center->setWidget(innerWindow);
}


/**
 * @brief MedeaMainWindow::setupWelcomeScreen
 */
void MainWindow::setupWelcomeScreen()
{
    if(!welcomeScreen){
        welcomeScreen = new WelcomeScreenWidget(view_controller->getActionController(), this);
    }
}


/**
 * @brief MedeaMainWindow::setupMenuBar
 */
void MainWindow::setupMenuBar()
{
    if(!menu_bar){
        menu_bar = new QMenuBar(this);
        menu_bar->setNativeMenuBar(false);

        menu_bar->installEventFilter(this);
        setMenuBar(menu_bar);
        
    
        //Add the required menus
        qCritical() << "ADDED: " << menu_bar->addMenu(action_controller->menu_file);
        menu_bar->addMenu(action_controller->menu_edit);
        menu_bar->addMenu(action_controller->menu_view);
        menu_bar->addMenu(action_controller->menu_model);
        menu_bar->addMenu(action_controller->menu_jenkins);
        menu_bar->addMenu(action_controller->menu_options);
        menu_bar->addMenu(action_controller->menu_help);

        //Construct and setup the correct style
        auto menu_style = new CustomMenuStyle(MENU_ICON_SIZE);
    
        for(auto action : menu_bar->actions()){
            auto menu = action->menu();
            if(menu){
                menu->setStyle(menu_style);
            }
        }
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


    auto applicationToolbar_spacer1 = new QWidget(applicationToolbar);
    auto applicationToolbar_spacer2 = new QWidget(applicationToolbar);
    applicationToolbar->addWidget(applicationToolbar_spacer1);
    applicationToolbar->addActions(view_controller->getActionController()->applicationToolbar->actions());
    applicationToolbar->addWidget(applicationToolbar_spacer2);

    
    connect(applicationToolbar, &QToolBar::orientationChanged, this, [applicationToolbar_spacer1, applicationToolbar_spacer2](Qt::Orientation orientation){
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
    });

    emit applicationToolbar->orientationChanged(Qt::Horizontal);
    innerWindow->addToolBar(applicationToolbar);
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
    

    auto notificationToolbar = NotificationManager::manager()->getToolbar();
    
    notificationToolbar->setParent(this); 

    //Set
    auto container_widget = new QFrame(this);
    container_widget->setStyleSheet("QFrame{padding:0px;border: 1px solid red; margin:0px;}");
    container_widget->setContentsMargins(0,0,0,0);
    

    auto toolbar = new QToolBar(this);
    toolbar->addAction(restoreToolsAction);

   // notificationToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    //toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);

    auto h_layout = new QHBoxLayout(container_widget);
    h_layout->setMargin(0);
    h_layout->setSpacing(2);
    h_layout->addWidget(notificationToolbar);
    h_layout->addWidget(toolbar);

    

    menu_bar->setCornerWidget(container_widget);
    

    //connect(restoreToolsAction, &QAction::triggered, this, &MainWindow::resetToolDockWidgets);
}

void MainWindow::updateMenuBar(){
    
}


/**
 * @brief MainWindow::setupDockablePanels
 * This sets up the search and notification dialogs.
 */
void MainWindow::setupDockablePanels()
{   
    //Setup the icons
    auto theme = Theme::theme();
    

    auto window_manager = WindowManager::manager();

    //QOS Browser
    dockwidget_Qos = window_manager->constructDockWidget("QOS Browser");
    dockwidget_Qos->setWidget(new QOSBrowser(view_controller, dockwidget_Qos));
    dockwidget_Qos->setIconVisible(true);
    dockwidget_Qos->setProtected(true);

    //Search Panel
    dockwidget_Search = window_manager->constructDockWidget("Search");
    dockwidget_Search->setWidget(SearchManager::manager()->getSearchDialog());
    dockwidget_Search->setIconVisible(true);
    dockwidget_Search->setProtected(true);
    
    //Notification Panel
    dockwidget_Notification = window_manager->constructDockWidget("Notifications");
    dockwidget_Notification->setWidget(NotificationManager::manager()->getPanel());
    dockwidget_Notification->setIconVisible(true);
    dockwidget_Notification->setProtected(true);

    //Jenkins Panel
    dockwidget_Jenkins = window_manager->constructDockWidget("Jenkins");
    dockwidget_Jenkins->setWidget(view_controller->getJenkinsManager()->GetJobMonitorWidget());
    
    dockwidget_Jenkins->setIconVisible(true);
    dockwidget_Jenkins->setProtected(true);

    // add tool dock widgets to the inner window
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dockwidget_Search);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dockwidget_Notification);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dockwidget_Qos);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dockwidget_Jenkins);

    // initially hide tool dock widgets
    innerWindow->setDockWidgetVisibility(dockwidget_Qos, false);
    innerWindow->setDockWidgetVisibility(dockwidget_Search, false);
    innerWindow->setDockWidgetVisibility(dockwidget_Notification, false);
    innerWindow->setDockWidgetVisibility(dockwidget_Jenkins, false);
    
    // Tab the search and notifications
    innerWindow->tabifyDockWidget(dockwidget_Search, dockwidget_Notification);
}


/**
 * @brief MedeaMainWindow::resizeToolWidgets
 */
void MainWindow::resizeToolWidgets()
{
    resizeDocks({dockwidget_Dock, dockwidget_Center, dockwidget_Right}, {3, 25, 4}, Qt::Horizontal);
    rightWindow->resizeDocks({dockwidget_Table, dockwidget_ViewManager, dockwidget_Minimap}, {2, 2, 1}, Qt::Vertical);
}   

/**
 * @brief MedeaMainWindow::closeEvent
 * @param event
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    //Save the state
    saveWindowState();
    if (view_controller) {
        view_controller->closeMEDEA();
        event->ignore();
    }
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if(object == menu_bar && event->type() == QEvent::Resize) {
        auto corner_widget = menu_bar->cornerWidget();
        if(corner_widget){
            auto size = menu_bar->actionGeometry(action_controller->menu_file->menuAction());
            qCritical() << "YO HOLMES" << size << " " << action_controller->menu_file->height();
            corner_widget->setFixedHeight(size.height());
            menu_bar->setFixedHeight(size.height() + 6);
        }
    }

    return QObject::eventFilter(object, event);
}