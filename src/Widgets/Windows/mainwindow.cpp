#include "mainwindow.h"
#include "../DockWidgets/viewdockwidget.h"
#include "../DockWidgets/nodeviewdockwidget.h"
#include "../DockWidgets/tooldockwidget.h"


#include "../../Controllers/SelectionController/selectioncontroller.h"
#include "../../Controllers/SettingsController/settingscontroller.h"

#include "../../Widgets/ViewManager/viewmanagerwidget.h"

#include "../../theme.h"

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

    initializeApplication();

    jenkinsManager = 0;
    cutsManager = 0;
    viewController = vc;

    setupTools();
    setupInnerWindow();
    setupJenkinsManager();
    setupCUTSManager();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(WindowManager::manager(), SIGNAL(activeViewDockWidgetChanged(ViewDockWidget*,ViewDockWidget*)), this, SLOT(activeViewDockWidgetChanged(ViewDockWidget*, ViewDockWidget*)));
    connect(NotificationManager::manager(), &NotificationManager::notificationAdded, this, &MainWindow::popupNotification);

    setViewController(vc);

    themeChanged();

    SettingsController* s = SettingsController::settings();

    int width = s->getSetting(SK_GENERAL_WIDTH).toInt();
    int height = s->getSetting(SK_GENERAL_HEIGHT).toInt();
    resize(width, height);
    resizeToolWidgets();

    setModelTitle();
    if(s->getSetting(SK_GENERAL_MAXIMIZED).toBool()){
        showMaximized();
    }else{
        showNormal();
    }

    toggleWelcomeScreen(true);
}


/**
 * @brief MedeaMainWindow::~MedeaMainWindow
 */
MainWindow::~MainWindow()
{
    cutsManager->deleteLater();

    saveSettings();

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

    connect(viewController, &ViewController::vc_backgroundProcess, NotificationManager::manager(), &NotificationManager::backgroundProcess);
    connect(viewController, &ViewController::vc_modelValidated, NotificationManager::manager(), &NotificationManager::modelValidated);

    connect(controller, &SelectionController::itemActiveSelectionChanged, tableWidget, &DataTableWidget::itemActiveSelectionChanged);

    connect(vc, &ViewController::mc_projectModified, this, &MainWindow::setWindowModified);
    connect(vc, &ViewController::vc_projectPathChanged, this, &MainWindow::setModelTitle);

    connect(vc, &ViewController::vc_showWelcomeScreen, this, &MainWindow::toggleWelcomeScreen);
    connect(vc, &ViewController::mc_projectModified, this, &MainWindow::setWindowModified);
    connect(actionController, &ActionController::recentProjectsUpdated, this, &MainWindow::recentProjectsUpdated);

    if (vc->getActionController()) {
        connect(vc->getActionController()->getRootAction("Root_Search"), SIGNAL(triggered(bool)), this, SLOT(popupSearch()));
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
        qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
        searchPanel->searchResults(query, viewController->getSearchResults(query));
        qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();
        qCritical() << "searchEntered in: " <<  timeFinish - timeStart << "MS";

        // make sure that the search panel dock widget is visible and that its window is raised
        // TODO - Make the search dock widget active
        if (!searchDockWidget->isVisible()) {
            searchDockWidget->req_Visible(searchDockWidget->getID(), true);
        }
        searchDockWidget->activateWindow();
    }
}


/**
 * @brief MainWindow::popupNotification
 * @param iconPath
 * @param iconName
 * @param description
 */
void MainWindow::popupNotification(QString iconPath, QString iconName, QString description)
{
    notificationPopup->hide();
    notificationTimer->stop();

    if (!welcomeScreenOn) {
        notificationLabel->setText(description);
        QPixmap pixmap = Theme::theme()->getIcon(iconPath, iconName).pixmap(QSize(32,32));
        if (pixmap.isNull()) {
            pixmap = Theme::theme()->getIcon("Actions", "Information").pixmap(QSize(32,32));
        }
        notificationIconLabel->setPixmap(pixmap);
        notificationPopup->setSize(notificationWidget->sizeHint().width() + 15, notificationWidget->sizeHint().height() + 10);
        moveWidget(notificationPopup, 0, Qt::AlignBottom);
        notificationPopup->show();
        notificationPopup->raise();
        notificationTimer->start(5000);
    }
}


/**
 * @brief MainWindow::toggleNotificationPanel
 * This toggles the visibility of the notification panel dock widget.
 * If it's already visible but its parent window is not active, activate the window instead of hiding it.
 */
void MainWindow::toggleNotificationPanel()
{
    if (notificationDockWidget->isVisible()) {
        if (notificationDockWidget->isActiveWindow()) {
            notificationDockWidget->req_Visible(notificationDockWidget->getID(), false);
        } else {
            notificationDockWidget->activateWindow();
        }
    } else {
        notificationDockWidget->req_Visible(notificationDockWidget->getID(), true);
        notificationDockWidget->activateWindow();
    }
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
 * @param value
 */
void MainWindow::updateProgressBar(int value)
{
    if (progressPopup->isVisible()) {
        if (value == -1) {
            progressBar->setRange(0,0);
        } else {
            progressBar->setRange(0,100);
            progressBar->setValue(value);
        }
    }
}


/**
 * @brief MainWindow::updateMenuBarSize
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
    foreach (BaseDockWidget* child, getDockWidgets()) {
        child->setVisible(true);
    }
}


/**
 * @brief MedeaMainWindow::themeChanged
 */
void MainWindow::themeChanged()
{
    Theme* theme = Theme::theme();

    QString menuStyle = theme->getMenuStyleSheet();
    ActionController* actionController = viewController->getActionController();

    actionController->menu_file->setStyleSheet(menuStyle);
    actionController->menu_file_recentProjects->setStyleSheet(menuStyle);
    actionController->menu_edit->setStyleSheet(menuStyle);
    actionController->menu_view->setStyleSheet(menuStyle);
    actionController->menu_model->setStyleSheet(menuStyle);
    actionController->menu_jenkins->setStyleSheet(menuStyle);
    actionController->menu_help->setStyleSheet(menuStyle);
    actionController->menu_window->setStyleSheet(menuStyle);
    actionController->menu_options->setStyleSheet(menuStyle);

    searchCompleter->popup()->setStyleSheet(theme->getAbstractItemViewStyleSheet() % theme->getScrollBarStyleSheet() % "QAbstractItemView::item{ padding: 2px 0px; }");
    searchPopup->setStyleSheet(theme->getPopupWidgetStyleSheet());
    searchToolbar->setStyleSheet(theme->getToolBarStyleSheet());
    searchBar->setStyleSheet(theme->getLineEditStyleSheet());
    searchButton->setIcon(theme->getIcon("Actions", "Search"));

    progressPopup->setStyleSheet(theme->getPopupWidgetStyleSheet());
    progressBar->setStyleSheet(theme->getProgressBarStyleSheet());
    progressLabel->setStyleSheet("background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() + ";");

    notificationPopup->setStyleSheet(theme->getPopupWidgetStyleSheet() + "QLabel{ background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() + "; }");
    //notificationLabel->setStyleSheet("background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() + ";");

    restoreToolsButton->setIcon(theme->getIcon("Actions", "Build"));
    restoreToolsAction->setIcon(theme->getIcon("Actions", "Refresh"));

    minimap->setStyleSheet(theme->getNodeViewStyleSheet());
}


/**
 * @brief MedeaMainWindow::activeViewDockWidgetChanged
 * @param viewDock
 * @param prevDock
 */
void MainWindow::activeViewDockWidgetChanged(ViewDockWidget *viewDock, ViewDockWidget *prevDock)
{
    if(viewDock){
        NodeViewDockWidget* nodeViewDock = 0;
        NodeView* view = 0;

        if(viewDock->isNodeViewDock()){
            nodeViewDock = (NodeViewDockWidget*) viewDock;
            view = nodeViewDock->getNodeView();
        }

        if(prevDock && prevDock->isNodeViewDock()){
            NodeViewDockWidget* prevNodeViewDock = (NodeViewDockWidget*) prevDock;
            NodeView* prevView = prevNodeViewDock->getNodeView();
            if(prevView){
                disconnect(minimap, &NodeViewMinimap::minimap_Pan, prevView, &NodeView::minimap_Pan);
                disconnect(minimap, &NodeViewMinimap::minimap_Zoom, prevView, &NodeView::minimap_Zoom);
                disconnect(prevView, &NodeView::sceneRectChanged, minimap, &NodeViewMinimap::sceneRectChanged);
                disconnect(prevView, &NodeView::viewportChanged, minimap, &NodeViewMinimap::viewportRectChanged);
            }
        }

        if(view){
            minimap->setBackgroundColor(view->getBackgroundColor());
            minimap->setScene(view->scene());

            connect(minimap, &NodeViewMinimap::minimap_Pan, view, &NodeView::minimap_Pan);
            connect(minimap, &NodeViewMinimap::minimap_Zoom, view, &NodeView::minimap_Zoom);
            connect(view, &NodeView::sceneRectChanged, minimap, &NodeViewMinimap::sceneRectChanged);
            connect(view, &NodeView::viewportChanged, minimap, &NodeViewMinimap::viewportRectChanged);

            view->forceViewportChange();
        }else{
            minimap->setBackgroundColor(QColor(0,0,0));
            minimap->setScene(0);
        }
    }
}


/**
 * @brief MedeaMainWindow::popupSearch
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
 * @brief MedeaMainWindow::toolbarChanged
 * @param area
 */
void MainWindow::toolbarChanged(Qt::DockWidgetArea area)
{
    if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) {
        applicationToolbar->setOrientation(Qt::Horizontal);
        applicationToolbar->setFixedHeight(QWIDGETSIZE_MAX);
        applicationToolbar->setFixedWidth(QWIDGETSIZE_MAX);
    } else {
        applicationToolbar->setOrientation(Qt::Vertical);
        resizeEvent(0);
    }
}


/**
 * @brief MedeaMainWindow::toolbarTopLevelChanged
 * @param undocked
 */
void MainWindow::toolbarTopLevelChanged(bool undocked)
{
    if (undocked) {
        if (applicationToolbar->orientation() == Qt::Vertical) {
            applicationToolbar->setOrientation(Qt::Horizontal);
            applicationToolbar->setFixedHeight(QWIDGETSIZE_MAX);
        }
        //applicationToolbar->parentWidget()->resize(applicationToolbar->sizeHint() +  QSize(12,0));
        QWidget* topWidget =  applicationToolbar->parentWidget()->parentWidget();
        if (topWidget) {
            //topWidget->resize(applicationToolbar->sizeHint() + QSize(15,0));
            topWidget->resize(applicationToolbar->sizeHint() + QSize(6,0));
            topWidget->updateGeometry();
        }
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
    QApplication::setApplicationVersion(APP_VERSION);
    QApplication::setOrganizationName("CDIT-MA");
    QApplication::setOrganizationDomain("https://github.com/cdit-ma/");
    QApplication::setWindowIcon(Theme::theme()->getIcon("Actions", "MEDEA"));

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

    // show/hide the menu bar and close all dock widgets
    menuBar->setVisible(!on);
    setDockWidgetsVisible(!on);

    if (on) {
        holderLayout->removeWidget(welcomeScreen);
        holderLayout->addWidget(innerWindow);
        setCentralWidget(welcomeScreen);
    } else {
        holderLayout->removeWidget(innerWindow);
        holderLayout->addWidget(welcomeScreen);
        setCentralWidget(innerWindow);
    }

    welcomeScreenOn = on;
}


/**
 * @brief MedeaMainWindow::saveSettings
 */
void MainWindow::saveSettings()
{
    SettingsController* s = SettingsController::settings();
    if(s && s->getSetting(SK_GENERAL_SAVE_WINDOW_ON_EXIT).toBool()){
        s->setSetting(SK_GENERAL_MAXIMIZED, isMaximized());
        if(!isMaximized()){
            s->setSetting(SK_GENERAL_WIDTH, width());
            s->setSetting(SK_GENERAL_HEIGHT, height());
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
    innerWindow = WindowManager::constructCentralWindow("Main Window");
    setCentralWidget(innerWindow);

    //Construct dockWidgets.
    NodeViewDockWidget* dwInterfaces = viewController->constructNodeViewDockWidget("Interface");
    NodeViewDockWidget* dwBehaviour = viewController->constructNodeViewDockWidget("Behaviour");
    NodeViewDockWidget* dwAssemblies = viewController->constructNodeViewDockWidget("Assemblies");
    NodeViewDockWidget* dwHardware = viewController->constructNodeViewDockWidget("Hardware");

    BaseDockWidget *dwQOSBrowser = WindowManager::constructViewDockWidget("QOS Browser");
    dwQOSBrowser->setWidget(new QOSBrowser(viewController, dwQOSBrowser));

    //Set each NodeView with there contained aspects
    dwInterfaces->getNodeView()->setContainedViewAspect(VA_INTERFACES);
    dwBehaviour->getNodeView()->setContainedViewAspect(VA_BEHAVIOUR);
    dwAssemblies->getNodeView()->setContainedViewAspect(VA_ASSEMBLIES);
    dwHardware->getNodeView()->setContainedViewAspect(VA_HARDWARE);

    //Set allowed areas
    dwInterfaces->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dwBehaviour->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dwAssemblies->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dwHardware->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dwQOSBrowser->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    //Set Icons
    dwInterfaces->setIcon("Items", "InterfaceDefinitions");
    dwBehaviour->setIcon("Items", "BehaviourDefinitions");
    dwAssemblies->setIcon("Items", "AssemblyDefinitions");
    dwHardware->setIcon("Items", "HardwareDefinitions");
    dwQOSBrowser->setIcon("Items", "QOSProfile");

    //Set Icon Visibility
    dwInterfaces->setIconVisible(false);
    dwBehaviour->setIconVisible(false);
    dwAssemblies->setIconVisible(false);
    dwHardware->setIconVisible(false);
    dwQOSBrowser->setIconVisible(true);

    //Protected from deletion
    dwInterfaces->setProtected(true);
    dwBehaviour->setProtected(true);
    dwAssemblies->setProtected(true);
    dwHardware->setProtected(true);
    dwQOSBrowser->setProtected(true);

    SettingsController* s = SettingsController::settings();

    //Set initial area
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dwInterfaces);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dwBehaviour);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dwAssemblies);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dwHardware);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dwQOSBrowser);

    innerWindow->setDockWidgetVisibility(dwInterfaces,   s->getSetting(SK_WINDOW_INTERFACES_VISIBLE).toBool());
    innerWindow->setDockWidgetVisibility(dwBehaviour,    s->getSetting(SK_WINDOW_BEHAVIOUR_VISIBLE).toBool());
    innerWindow->setDockWidgetVisibility(dwAssemblies,   s->getSetting(SK_WINDOW_ASSEMBLIES_VISIBLE).toBool());
    innerWindow->setDockWidgetVisibility(dwHardware,     s->getSetting(SK_WINDOW_HARDWARE_VISIBLE).toBool());
    innerWindow->setDockWidgetVisibility(dwQOSBrowser,  s->getSetting(SK_WINDOW_QOS_VISIBLE).toBool());

    // NOTE: Apparently calling innerWindow's createPopupMenu crashes the
    // application if it's called before the dock widgets are added above
    // This function needs to be called after the code above and before the connections below
    setupMenuCornerWidget();
    setupDockablePanels();
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
    menuBar->addMenu(ac->menu_window);
    menuBar->addMenu(ac->menu_options);
    menuBar->addMenu(ac->menu_help);
}


/**
 * @brief MedeaMainWindow::setupToolBar
 */
void MainWindow::setupToolBar()
{
    applicationToolbar = new QToolBar(this);
    applicationToolbar->setMovable(false);
    applicationToolbar->setFloatable(false);
    applicationToolbar->setIconSize(QSize(20,20));

    QWidget* w1 = new QWidget(this);
    QWidget* w2 = new QWidget(this);
    w1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    w2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QFrame* frame = new QFrame(this);
    QHBoxLayout* layout = new QHBoxLayout(frame);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(w1);
    layout->addWidget(applicationToolbar);
    layout->addWidget(w2);

    applicationToolbar->addActions(viewController->getActionController()->applicationToolbar->actions());

    BaseDockWidget* dockWidget = WindowManager::constructToolDockWidget("Toolbar");
    dockWidget->setTitleBarWidget(frame);
    dockWidget->setAllowedAreas(Qt::TopDockWidgetArea);

    connect(dockWidget, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(toolbarChanged(Qt::DockWidgetArea)));
    connect(dockWidget, SIGNAL(topLevelChanged(bool)), this, SLOT(toolbarTopLevelChanged(bool)));

    //Check visibility state.
    dockWidget->setVisible(SettingsController::settings()->getSetting(SK_WINDOW_TOOLBAR_VISIBLE).toBool());
    addDockWidget(Qt::TopDockWidgetArea, dockWidget, Qt::Horizontal);
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

    notificationPopup = new PopupWidget(PopupWidget::TOOL, 0);
    notificationPopup->setWidget(notificationWidget);
    notificationPopup->hide();

    notificationTimer = new QTimer(this);
    connect(notificationTimer, &QTimer::timeout, notificationPopup, &QDialog::hide);
}


/**
 * @brief MedeaMainWindow::setupDock
 */
void MainWindow::setupDock()
{
    dockTabWidget = new DockTabWidget(viewController, this);

    BaseDockWidget* dockWidget = WindowManager::constructToolDockWidget("Dock");
    dockWidget->setWidget(dockTabWidget);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea);

    connect(viewController->getActionController()->toggleDock, SIGNAL(triggered(bool)), dockWidget, SLOT(setVisible(bool)));

    //Check visibility state.
    //dockWidget->setVisible(SettingsController::settings()->getSetting(SK_WINDOW_TABLE_VISIBLE).toBool());
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget, Qt::Vertical);
}


/**
 * @brief MedeaMainWindow::setupDataTable
 */
void MainWindow::setupDataTable()
{
    BaseDockWidget* dockWidget = WindowManager::constructToolDockWidget("Table");
    tableWidget = new DataTableWidget(viewController, dockWidget);
    dockWidget->setWidget(tableWidget);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    //Add the rename action to the dockwidget so that it'll handle rename shortcut
    dockWidget->addAction(viewController->getActionController()->edit_renameActiveSelection);

    QAction* modelAction = viewController->getActionController()->model_selectModel;
    modelAction->setToolTip("Show Model's Table");
    dockWidget->getTitleBar()->addToolAction(modelAction, Qt::AlignLeft);

    //Check visibility state.
    addDockWidget(Qt::RightDockWidgetArea, dockWidget, Qt::Vertical);
    setDockWidgetVisibility(dockWidget, SettingsController::settings()->getSetting(SK_WINDOW_TABLE_VISIBLE).toBool());
}


/**
 * @brief MedeaMainWindow::setupMinimap
 */
void MainWindow::setupMinimap()
{
    minimap = new NodeViewMinimap(this);
    minimap->setMinimumHeight(150);

    BaseDockWidget* dockWidget = WindowManager::constructToolDockWidget("Minimap");
    dockWidget->setWidget(minimap);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    addDockWidget(Qt::RightDockWidgetArea, dockWidget, Qt::Vertical);
    setDockWidgetVisibility(dockWidget, SettingsController::settings()->getSetting(SK_WINDOW_MINIMAP_VISIBLE).toBool());
}


/**
 * @brief MedeaMainWindow::setupMenuCornerWidget
 * NOTE: This neeeds to be called after the tool dock widgets
 * and both the central and inner windows are constructed.
 */
void MainWindow::setupMenuCornerWidget()
{
    QMenu* menu = createPopupMenu();
    restoreToolsAction = menu->addAction("Show All Tool Widgets");

    restoreToolsButton = new QToolButton(this);
    restoreToolsButton->setToolTip("Restore Tool Dock Widgets");
    restoreToolsButton->setMenu(menu);
    restoreToolsButton->setPopupMode(QToolButton::InstantPopup);
    restoreToolsButton->setStyleSheet("QToolButton{ border-radius: 4px; }"
                                      "QToolButton::menu-indicator{ image: none; }");

    notificationToolbar = new NotificationToolbar(viewController, this);

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

    connect(NotificationManager::manager(), &NotificationManager::updateNotificationToolbarSize, this, &MainWindow::updateMenuBarSize);
    connect(restoreToolsAction, SIGNAL(triggered(bool)), this, SLOT(resetToolDockWidgets()));
}


/**
 * @brief MainWindow::setupDockablePanels
 * This sets up the search and notification dialogs.
 */
void MainWindow::setupDockablePanels()
{
    searchPanel = new SearchDialog(this);
    searchDockWidget = WindowManager::constructViewDockWidget("Search Results");
    searchDockWidget->setWidget(searchPanel);
    searchDockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    searchDockWidget->setIcon("Actions", "Search_Icon");
    searchDockWidget->setIconVisible(true);
    searchDockWidget->setProtected(true);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, searchDockWidget);

    notificationPanel = new NotificationDialog(this);
    notificationDockWidget = WindowManager::constructViewDockWidget("Notifications");
    notificationDockWidget->setWidget(notificationPanel);
    notificationDockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    notificationDockWidget->setIcon("Actions", "Notification");
    notificationDockWidget->setIconVisible(true);
    notificationDockWidget->setProtected(true);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, notificationDockWidget);

    // initially hide tool dock widgets
    innerWindow->setDockWidgetVisibility(searchDockWidget, false);
    innerWindow->setDockWidgetVisibility(notificationDockWidget, false);

    if (viewController) {
        connect(viewController, &ViewController::vc_setupModel, searchPanel, &SearchDialog::resetPanel);
        connect(searchPanel, SIGNAL(centerOnViewItem(int)), viewController, SLOT(centerOnID(int)));
        connect(searchPanel, SIGNAL(popupViewItem(int)), viewController, SLOT(popupItem(int)));
        connect(searchPanel, SIGNAL(itemHoverEnter(int)), viewController->getToolbarController(), SLOT(actionHoverEnter(int)));
        connect(searchPanel, SIGNAL(itemHoverLeave(int)), viewController->getToolbarController(), SLOT(actionHoverLeave(int)));
        connect(notificationPanel, &NotificationDialog::centerOn, viewController, &ViewController::centerOnID);
        connect(notificationPanel, &NotificationDialog::popup, viewController, &ViewController::popupItem);
    }
    if (notificationToolbar) {
        connect(notificationPanel, &NotificationDialog::updateSeverityCount, notificationToolbar, &NotificationToolbar::updateSeverityCount);
        connect(notificationPanel, &NotificationDialog::mouseEntered, notificationToolbar, &NotificationToolbar::notificationsSeen);
        connect(notificationToolbar, &NotificationToolbar::toggleDialog, this, &MainWindow::toggleNotificationPanel);
    }
    connect(searchPanel, &SearchDialog::searchButtonClicked, this, &MainWindow::popupSearch);
    connect(searchPanel, &SearchDialog::refreshButtonClicked, this, &MainWindow::searchEntered);
    connect(NotificationManager::manager(), &NotificationManager::showNotificationPanel, this, &MainWindow::toggleNotificationPanel);
}


/**
 * @brief MedeaMainWindow::setupViewManager
 */
void MainWindow::setupViewManager()
{
    viewManager = WindowManager::manager()->getViewManagerGUI();
    viewManager->setMinimumHeight(210);

    BaseDockWidget* dockWidget = WindowManager::constructToolDockWidget("View Manager");
    dockWidget->setWidget(viewManager);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    addDockWidget(Qt::RightDockWidgetArea, dockWidget, Qt::Vertical);
    setDockWidgetVisibility(dockWidget, SettingsController::settings()->getSetting(SK_WINDOW_VIEW_MANAGER_VISIBLE).toBool());
}


/**
 * @brief MedeaMainWindow::setupJenkinsManager
 */
void MainWindow::setupJenkinsManager()
{
    if(!jenkinsManager){
        jenkinsManager = new JenkinsManager(this);
        connect(jenkinsManager, &JenkinsManager::settingsValidationComplete, viewController, &ViewController::jenkinsManager_SettingsValidated);
        connect(jenkinsManager, &JenkinsManager::gotValidJava, viewController, &ViewController::jenkinsManager_GotJava);


        connect(viewController->getActionController()->jenkins_importNodes, &QAction::triggered, jenkinsManager, &JenkinsManager::getJenkinsNodes);

        connect(jenkinsManager, &JenkinsManager::gotJenkinsNodeGraphml, viewController, &ViewController::jenkinsManager_GotJenkinsNodesList);
        connect(jenkinsManager, &JenkinsManager::jenkinsReady, viewController, &ViewController::vc_JenkinsReady);

        connect(viewController, &ViewController::vc_executeJenkinsJob, jenkinsManager, &JenkinsManager::executeJenkinsJob);

        jenkinsManager->validateSettings();
    }
}


/**
 * @brief MedeaMainWindow::setupCUTSManager
 */
void MainWindow::setupCUTSManager()
{
    if(!cutsManager ){
        cutsManager  = new CUTSManager();
        xmiImporter = new XMIImporter(cutsManager, this);

        //connect(cutsManager, &CUTSManager::localDeploymentOkay, viewController, &ViewController::cutsManager_DeploymentOkay);
        connect(viewController, &ViewController::vc_getCodeForComponent, cutsManager, &CUTSManager::getCPPForComponent);
        connect(viewController, &ViewController::vc_importXMEProject, cutsManager, &CUTSManager::executeXMETransform);

        connect(viewController, &ViewController::vc_validateModel, cutsManager, &CUTSManager::executeXSLValidation);
        connect(viewController, &ViewController::vc_launchLocalDeployment, cutsManager, &CUTSManager::showLocalDeploymentGUI, Qt::DirectConnection);

        connect(cutsManager, &CUTSManager::gotCodeForComponent, viewController, &ViewController::showCodeViewer);
        connect(cutsManager, &CUTSManager::gotXMETransform, viewController, &ViewController::importGraphMLFile);
        connect(cutsManager, &CUTSManager::executedXSLValidation, viewController, &ViewController::modelValidated);

        connect(viewController, &ViewController::vc_importXMIProject, xmiImporter, &XMIImporter::importXMI);
        connect(xmiImporter, &XMIImporter::loadingStatus, this, &MainWindow::showProgressBar);
        connect(xmiImporter, &XMIImporter::gotXMIGraphML, viewController, &ViewController::importGraphMLExtract);
    }
}


/**
 * @brief MedeaMainWindow::resizeToolWidgets
 */
void MainWindow::resizeToolWidgets()
{
    int windowHeight = height();
    int defaultWidth = 500;
    tableWidget->resize(defaultWidth, windowHeight/2);
    tableWidget->updateGeometry();
    minimap->parentWidget()->resize(defaultWidth, windowHeight / 4);
    viewManager->parentWidget()->resize(defaultWidth, windowHeight / 4);
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
        cw = QApplication::activeWindow();
        if (!cw->isWindowType()) {
            cw = WindowManager::manager()->getActiveWindow();
            qDebug() << "Current widget is not a window!";
        } else {
            qDebug() << "HERE - " << cw;
        }
        widgetPos.ry() -= widget->height()/2 + 8;
    }
    if (cw == innerWindow) {
        widgetPos = pos() - QPointF(0, 8);
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
    if (viewController) {
        viewController->closeMEDEA();
        event->ignore();
    }
}
