#include "mainwindow.h"
#include "welcomescreenwidget.h"

#include "../../theme.h"
#include "../DockWidgets/viewdockwidget.h"
#include "../DockWidgets/tooldockwidget.h"
#include "../DockWidgets/invisibledockwidget.h"

#include "../../Widgets/Dialogs/popupwidget.h"
#include "../../Widgets/Dialogs/progresspopup.h"
#include "../../Widgets/ViewManager/viewmanagerwidget.h"
//#include "../../Widgets/Charts/Timeline/chartdialog.h"
#include "../../Widgets/Charts/ChartManager/chartmanager.h"

#include "../../Controllers/SearchManager/searchmanager.h"
#include "../../Controllers/JenkinsManager/jenkinsmanager.h"
#include "../../Controllers/SettingsController/settingscontroller.h"
#include "../../Controllers/NotificationManager/notificationobject.h"
#include "../../Controllers/SelectionController/selectioncontroller.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"

#include "../../Views/NodeView/nodeview.h"
#include "../../Views/Dock/docktabwidget.h"
#include "../../Views/Search/searchdialog.h"
#include "../../Views/Table/datatablewidget.h"
#include "../../Views/QOSBrowser/qosbrowser.h"
#include "../../Views/NodeView/nodeviewminimap.h"
#include "../../Views/Notification/notificationdialog.h"
#include "../../Views/Notification/notificationtoolbar.h"

#include <QDebug>
#include <QMenuBar>
#include <QApplication>
#include <QStringBuilder>
#include <QDesktopWidget>
#include <QShortcut>
#define MENU_ICON_SIZE 16


/**
 * @brief MedeaMainWindow::MedeaMainWindow
 * @param vc
 * @param parent
 */
MainWindow::MainWindow(ViewController* view_controller, QWidget* parent):BaseWindow(parent, BaseWindow::MAIN_WINDOW)
{
    
    initializeApplication();
    setViewController(view_controller);
    
    if(!reset_action){
        reset_action = new QAction("Reset Tool Dock Widgets", this);
        connect(reset_action, &QAction::triggered, this, &MainWindow::resetToolDockWidgets);
    }
    //Setup Welcome screen
    if(!welcomeScreen){
        welcomeScreen = new WelcomeScreenWidget(view_controller->getActionController(), this);
        swapCentralWidget(welcomeScreen);
    }
    
    

    setupTools();
    setupInnerWindow();
    
    addDockWidget(Qt::BottomDockWidgetArea, dockwidget_Dock);
    addDockWidget(Qt::BottomDockWidgetArea, dockwidget_Center);
    addDockWidget(Qt::BottomDockWidgetArea, dockwidget_Right);

    setupDockIcons();
    
   
    connect(Theme::theme(), &Theme::theme_Changed, this, &MainWindow::themeChanged);
    connect(this, &MainWindow::welcomeScreenToggled, view_controller, &ViewController::welcomeScreenToggled);

    connect(SearchManager::manager(), &SearchManager::SearchComplete, this, [=](){WindowManager::ShowDockWidget(dockwidget_Search);});
    connect(NotificationManager::manager(), &NotificationManager::showNotificationPanel, this, [=](){WindowManager::ShowDockWidget(dockwidget_Notification);});

    SettingsController* s = SettingsController::settings();

    
    auto outer_geo = s->getSetting(SETTINGS::WINDOW_OUTER_GEOMETRY).toByteArray();
    if(!outer_geo.isEmpty()){
        restoreGeometry(outer_geo);
    }

    setModelTitle();
    themeChanged();
    
    toggleWelcomeScreen(true);
    activateWindow();
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

    connect(view_controller, &ViewController::ProjectModified, this, &MainWindow::setWindowModified);
    connect(view_controller, &ViewController::ProjectFileChanged, this, &MainWindow::setModelTitle);
    connect(view_controller, &ViewController::vc_showWelcomeScreen, this, &MainWindow::toggleWelcomeScreen);

    addActions(action_controller->getAllActions());
}

/**
 * @brief MedeaMainWindow::resetToolDockWidgets
 */
void MainWindow::resetToolDockWidgets()
{
    
    resetDockWidgets();
    rightWindow->resetDockWidgets();

    innerWindow->addToolBar(applicationToolbar);
    applicationToolbar->setVisible(true);
    resetToolWidgets();
}

bool MainWindow::isWelcomeScreenVisible(){
    return centralWidget() == welcomeScreen;
}

QMenu* MainWindow::createPopupMenu(){
    QMenu* menu = new QMenu(this);
    if(isWelcomeScreenVisible()){
        return menu;
    }

    auto inner_docks = innerWindow->getDockWidgets();
    std::sort(inner_docks.begin(), inner_docks.end(), &WindowManager::Sort);

    for(auto dock_widget : inner_docks){
        menu->addAction(dock_widget->toggleViewAction());
    }
    menu->addSeparator();
    
    menu->addAction(dockwidget_Dock->toggleViewAction());
    
    auto tool_docks = rightWindow->getDockWidgets();
    tool_docks.append(dockwidget_Dock);

    std::sort(tool_docks.begin(), tool_docks.end(), &WindowManager::Sort);
    for(auto dock_widget : tool_docks){
        menu->addAction(dock_widget->toggleViewAction());
    }
    menu->addAction(applicationToolbar->toggleViewAction());

    menu->addSeparator();
    menu->addAction(reset_action);
    return menu;
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

    auto icon_size = theme->getIconSize();

    menu_bar->setStyleSheet(theme->getMenuBarStyleSheet());

    auto menu_style = new CustomMenuStyle(icon_size.width());
    QString menuStyle = theme->getMenuStyleSheet(icon_size.width());
    
    for(auto action : menu_bar->actions()){
        auto menu = action->menu();
        if(menu){
            menu->setStyle(menu_style);
            menu->setStyleSheet(menuStyle);
        }
    }

    
    applicationToolbar->toggleViewAction()->setIcon(theme->getIcon("WindowIcon", applicationToolbar->windowTitle()));

    restore_toolbutton->setStyleSheet("QToolButton::menu-indicator{image: none; }");
    restore_toolbutton->setIcon(theme->getIcon("Icons", "spanner"));
    
    if(reset_action){
        reset_action->setIcon(theme->getIcon("Icons", "refresh"));
    }

    applicationToolbar->setIconSize(theme->getIconSize());
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
    QString title = "MEDEA " % model_title % "[*]";
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
    auto central_widget = centralWidget();
    if(central_widget){
        central_widget->setParent(0);
    }
    setCentralWidget(widget);
}


void MainWindow::toggleDocks(bool on){
    menu_bar->setVisible(on);
    setDockWidgetsVisible(on);
    rightWindow->setDockWidgetsVisible(on);
}
/**
 * @brief MedeaMainWindow::toggleWelcomeScreen
 * @param on
 */
void MainWindow::toggleWelcomeScreen(bool on)
{
    auto welcome_screen_on = isWelcomeScreenVisible();

    toggleDocks(!on);
    
    if(welcome_screen_on != on){
        //Swap between the welcome scree and 0
        swapCentralWidget(on ? welcomeScreen : 0);
        
        if(!on){
            restoreWindowState();
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

void MainWindow::restoreWindowState(){
    SettingsController* s = SettingsController::settings();
    if(s){
        auto load_dock = s->getSetting(SETTINGS::GENERAL_SAVE_DOCKS_ON_EXIT).toBool();
        auto load_window = s->getSetting(SETTINGS::GENERAL_SAVE_WINDOW_ON_EXIT).toBool();

        auto outer_state = s->getSetting(SETTINGS::WINDOW_OUTER_STATE).toByteArray();
        auto inner_state = s->getSetting(SETTINGS::WINDOW_INNER_STATE).toByteArray();
        auto right_state = s->getSetting(SETTINGS::WINDOW_RIGHT_STATE).toByteArray();

        //Check if any are invalid
        bool invalid = outer_state.isEmpty() || inner_state.isEmpty() || right_state.isEmpty();

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
    setDockWidgetIcon(dockwidget_Notification, "Icons", "bell", theme);
    setDockWidgetIcon(dockwidget_Dock, "Icons", "zoomInPage", theme);
    setDockWidgetIcon(dockwidget_Charts, "Icons", "barChart", theme);

    theme->setWindowIcon(applicationToolbar->windowTitle(), "Icons", "spanner");

    

}

/**
 * @brief MedeaMainWindow::setupTools
 */
void MainWindow::setupTools()
{
    //Setup Progress Bar
    auto progress_bar = new ProgressPopup();
    connect(view_controller, &ViewController::ShowProgress, progress_bar, &ProgressPopup::ProgressUpdated);
    connect(view_controller, &ViewController::ProgressUpdated, progress_bar, &ProgressPopup::UpdateProgressBar);
   
    setupMenuBar();

    auto window_manager = WindowManager::manager();
    
    if(!dockwidget_Dock){
        dockwidget_Dock = window_manager->constructToolDockWidget("Dock", this);
        dockwidget_Dock->setWidget(new DockTabWidget(view_controller, this));
    }

    if(!dockwidget_Table){
        dockwidget_Table = window_manager->constructToolDockWidget("Data Table", this);
        dockwidget_Table->setWidget(new DataTableWidget(view_controller, dockwidget_Table));
    }

    if(!dockwidget_Minimap){
        dockwidget_Minimap = window_manager->constructToolDockWidget("Minimap", this);
        dockwidget_Minimap->setWidget(new NodeViewMinimap(this));
    }

    if(!dockwidget_ViewManager){
        dockwidget_ViewManager = window_manager->constructToolDockWidget("View Manager", this);
        dockwidget_ViewManager->setWidget(window_manager->getViewManagerGUI());
    }

    if(!rightWindow){
        rightWindow = window_manager->constructInvisibleWindow("Right Tools", this);
        rightWindow->setDockNestingEnabled(true);
    
        rightWindow->addDockWidget(Qt::TopDockWidgetArea, dockwidget_Table, Qt::Vertical);
        rightWindow->addDockWidget(Qt::TopDockWidgetArea, dockwidget_ViewManager, Qt::Vertical);
        rightWindow->addDockWidget(Qt::TopDockWidgetArea, dockwidget_Minimap, Qt::Vertical);
    
        dockwidget_Right = window_manager->constructInvisibleDockWidget("Right Tools", this);
        dockwidget_Right->setWidget(rightWindow);
    }
}


/**
 * @brief MedeaMainWindow::setupInnerWindow
 */
void MainWindow::setupInnerWindow()
{
    innerWindow = WindowManager::manager()->constructCentralWindow("Main Window", this);

    //Construct dockWidgets.
    auto dockwidget_Interfaces = view_controller->constructViewDockWidget(getViewAspectName(VIEW_ASPECT::INTERFACES), this);
    auto dockwidget_Behaviour = view_controller->constructViewDockWidget(getViewAspectName(VIEW_ASPECT::BEHAVIOUR), this);
    auto dockwidget_Assemblies = view_controller->constructViewDockWidget(getViewAspectName(VIEW_ASPECT::ASSEMBLIES), this);
    auto dockwidget_Hardware = view_controller->constructViewDockWidget(getViewAspectName(VIEW_ASPECT::HARDWARE), this);
    
    

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

    /*//Uncomment for Workers View Aspect
    auto dockwidget_Workers = view_controller->constructViewDockWidget(getViewAspectName(VIEW_ASPECT::WORKERS), this);
    dockwidget_Workers->getNodeView()->setContainedViewAspect(VIEW_ASPECT::WORKERS);
    setDockWidgetIcon(dockwidget_Workers, "EntityIcons", "WorkerDefinitions", theme);
    dockwidget_Workers->setIconVisible(false);
    dockwidget_Workers->setProtected(true);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dockwidget_Workers);*/

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

    dockwidget_Center = WindowManager::manager()->constructInvisibleDockWidget("Central Widget", this);
    dockwidget_Center->setWidget(innerWindow);


    // TESTING INNER DOCK WIDGETS
    /*
    WindowManager::manager()->constructInnerDockWidget(view_controller, dockwidget_Behaviour, "Inner Dock Widget");
    dockwidget_Interfaces->req_Visible(dockwidget_Interfaces->getID(), false);
    dockwidget_Hardware->req_Visible(dockwidget_Hardware->getID(), false);
    */
    dockwidget_Assemblies->req_Visible(dockwidget_Assemblies->getID(), false);
}



/**
 * @brief MedeaMainWindow::setupMenuBar
 */
void MainWindow::setupMenuBar()
{
    if(!menu_bar){
        menu_bar = new QMenuBar(this);
        menu_bar->setNativeMenuBar(false);

        //Add the required menus
        menu_bar->addMenu(action_controller->menu_file);
        menu_bar->addMenu(action_controller->menu_edit);
        menu_bar->addMenu(action_controller->menu_view);
        menu_bar->addMenu(action_controller->menu_model);
        menu_bar->addMenu(action_controller->menu_jenkins);
        menu_bar->addMenu(action_controller->menu_options);
        menu_bar->addMenu(action_controller->menu_help);
        setMenuBar(menu_bar);
    }
}

/**
 * @brief MedeaMainWindow::setupToolBar
 */
void MainWindow::setupToolBar()
{
    if(!applicationToolbar){
        applicationToolbar = new QToolBar("Toolbar", this);
        applicationToolbar->setObjectName("APPLICATION_TOOLBAR");
        applicationToolbar->setMovable(true);
        applicationToolbar->setFloatable(true);

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
}


void MainWindow::setupMenuCornerWidget()
{
    auto notificationToolbar = NotificationManager::manager()->getToolbar();

    restore_toolbutton = new QToolButton(this);
    restore_toolbutton->setToolTip("Restore Tool Dock Widgets");
    restore_toolbutton->setObjectName("RIGHT_ACTION");
    restore_toolbutton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    

    connect(restore_toolbutton, &QToolButton::clicked, [=](){
        auto menu = createPopupMenu();
        restore_toolbutton->setMenu(menu);
        restore_toolbutton->showMenu();
        delete menu;
    });

    //Add in the Restore_toolbutton
    notificationToolbar->addWidget(restore_toolbutton);
    menu_bar->setCornerWidget(notificationToolbar);
}


/**
 * @brief MainWindow::setupDockablePanels
 * This sets up the search and notification dialogs.
 */
void MainWindow::setupDockablePanels()
{   
    auto window_manager = WindowManager::manager();

    //QOS Browser
    dockwidget_Qos = window_manager->constructDockWidget("QOS Browser", this);
    dockwidget_Qos->setWidget(new QOSBrowser(view_controller, dockwidget_Qos));
    dockwidget_Qos->setIconVisible(true);
    dockwidget_Qos->setProtected(true);

    //Search Panel
    dockwidget_Search = window_manager->constructDockWidget("Search", this);
    dockwidget_Search->setWidget(SearchManager::manager()->getSearchDialog());
    dockwidget_Search->setIconVisible(true);
    dockwidget_Search->setProtected(true);
    
    //Notification Panel
    dockwidget_Notification = window_manager->constructDockWidget("Notifications", this);
    dockwidget_Notification->setWidget(NotificationManager::manager()->getPanel());
    dockwidget_Notification->setIconVisible(true);
    dockwidget_Notification->setProtected(true);

    // Charts Panel
    //dockwidget_Charts = window_manager->constructChartDockWidget("Charts", new ChartDialog(view_controller, dockwidget_Charts), this);
    dockwidget_Charts = window_manager->constructChartDockWidget("Charts", ChartManager::manager()->getChartDialog(), this);
    dockwidget_Charts->setIconVisible(true);
    dockwidget_Charts->setProtected(true);

    // add tool dock widgets to the inner window
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dockwidget_Search);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dockwidget_Notification);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dockwidget_Qos);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dockwidget_Charts);

    // initially hide tool dock widgets
    innerWindow->setDockWidgetVisibility(dockwidget_Qos, false);
    innerWindow->setDockWidgetVisibility(dockwidget_Search, false);
    innerWindow->setDockWidgetVisibility(dockwidget_Notification, false);
    innerWindow->setDockWidgetVisibility(dockwidget_Charts, false);
    
    // Tab the search and notifications
    innerWindow->tabifyDockWidget(dockwidget_Search, dockwidget_Notification);
}


/**
 * @brief MedeaMainWindow::resizeToolWidgets
 */
void MainWindow::resetToolWidgets()
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
