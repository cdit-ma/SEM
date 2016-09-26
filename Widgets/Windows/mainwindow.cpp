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
    qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();


    initializeApplication();

    applicationToolbar = 0;
    jenkinsManager = 0;
    cutsManager = 0;
    viewController = vc;

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    setupTools(); //861MS
    setupInnerWindow(); //718
    setupJenkinsManager();
    setupCUTSManager();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(WindowManager::manager(), SIGNAL(activeViewDockWidgetChanged(ViewDockWidget*,ViewDockWidget*)), this, SLOT(activeViewDockWidgetChanged(ViewDockWidget*, ViewDockWidget*)));
    setViewController(vc);

    themeChanged();
    qint64 time2 = QDateTime::currentDateTime().toMSecsSinceEpoch();

    SettingsController* s = SettingsController::settings();

    int width = s->getSetting(SK_GENERAL_WIDTH).toInt();
    int height = s->getSetting(SK_GENERAL_HEIGHT).toInt();
    resize(width, height);

    if(SettingsController::settings()->getSetting(SK_GENERAL_MAXIMIZED).toBool()){
        showMaximized();
    }else{
        showNormal();
    }

    qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();

    toggleWelcomeScreen(true);

    qCritical() << "MedeaMainWindow in: " <<  time2 - timeStart << "MS";
    qCritical() << "MedeaMainWindow->show() in: " <<  timeFinish - time2 << "MS";
    setModelTitle("");

    resizeToolWidgets();
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

    connect(controller, &SelectionController::itemActiveSelectionChanged, tableWidget, &DataTableWidget::itemActiveSelectionChanged);

    connect(vc, &ViewController::mc_projectModified, this, &MainWindow::setWindowModified);
    connect(vc, &ViewController::vc_projectPathChanged, this, &MainWindow::setModelTitle);
    connect(vc, &ViewController::vc_showNotification, this, &MainWindow::showNotification);

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
        searchDialog->searchResults(query, viewController->getSearchResults(query));
        qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();
        searchDialog->show();
        qCritical() << "searchEntered in: " <<  timeFinish - timeStart << "MS";
    }
}


/**
 * @brief MedeaMainWindow::showNotification
 * @param title
 * @param message
 */
void MainWindow::showNotification(NOTIFICATION_TYPE type, QString title, QString description, QString iconPath, QString iconName, int ID)
{
    notificationDialog->addNotificationItem(type, title, description, QPair<QString, QString>(iconPath, iconName), ID);
    notificationTimer->stop();

    if (!welcomeScreenOn) {
        notificationLabel->setText(description);
        QPixmap pixmap = Theme::theme()->getIcon(iconPath, iconName).pixmap(QSize(32,32));
        if (pixmap.isNull()) {
            pixmap = Theme::theme()->getIcon("Actions", "Info").pixmap(QSize(32,32));
        }
        notificationIconLabel->setPixmap(pixmap);
        notificationPopup->setSize(notificationWidget->sizeHint().width() + 15, notificationWidget->sizeHint().height() + 10);
        moveWidget(notificationPopup, 0, Qt::AlignBottom);
        notificationPopup->show();
        notificationTimer->start(5000);
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
        moveWidget(progressPopup, this, alignment);
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
    viewController->getActionController()->menu_file->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_file_recentProjects->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_edit->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_view->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_model->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_jenkins->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_help->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_window->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_options->setStyleSheet(menuStyle);

    searchCompleter->popup()->setStyleSheet(theme->getAbstractItemViewStyleSheet() % theme->getScrollBarStyleSheet() % "QAbstractItemView::item{ padding: 2px 0px; }");
    searchPopup->setStyleSheet(theme->getPopupWidgetStyleSheet());
    searchToolbar->setStyleSheet(theme->getToolBarStyleSheet());
    searchBar->setStyleSheet(theme->getLineEditStyleSheet());
    searchButton->setIcon(theme->getIcon("Actions", "Search"));

    progressPopup->setStyleSheet(theme->getPopupWidgetStyleSheet());
    progressBar->setStyleSheet(theme->getProgressBarStyleSheet());
    progressLabel->setStyleSheet("background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() + ";");

    notificationPopup->setStyleSheet(theme->getPopupWidgetStyleSheet());
    notificationLabel->setStyleSheet("background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() + ";");

    restoreAspectsButton->setIcon(theme->getIcon("Actions", "MenuView"));
    restoreToolsButton->setIcon(theme->getIcon("Actions", "Build"));
    restoreToolsAction->setIcon(theme->getIcon("Actions", "Refresh"));

    interfaceButton->setStyleSheet(theme->getAspectButtonStyleSheet(VA_INTERFACES));
    behaviourButton->setStyleSheet(theme->getAspectButtonStyleSheet(VA_BEHAVIOUR));
    assemblyButton->setStyleSheet(theme->getAspectButtonStyleSheet(VA_ASSEMBLIES));
    hardwareButton->setStyleSheet(theme->getAspectButtonStyleSheet(VA_HARDWARE));

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
            topWidget->resize(applicationToolbar->sizeHint() + QSize(15,0));
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

    searchDialog->searchResults("ment", viewController->getSearchResults("ment"));
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

    //Set Font.
    //int opensans_FontID = QFontDatabase::addApplicationFont(":/Resources/Fonts/OpenSans-Regular.ttf");
    //QString opensans_fontname = QFontDatabase::applicationFontFamilies(opensans_FontID).at(0);
    //QFont font = QFont(opensans_fontname);

    QFont font("Verdana");
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPointSizeF(8.5);
    QApplication::setFont(font);
}


/**
 * @brief MedeaMainWindow::connectNodeView
 * @param nodeView
 */
void MainWindow::connectNodeView(NodeView *nodeView)
{
    if(nodeView && viewController){
        nodeView->setViewController(viewController);
    }
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
    qint64 t1 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupWelcomeScreen();
    qint64 t2 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupMenuBar();
    qint64 t3 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupSearchBar();
    qint64 t4 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupProgressBar();
    qint64 t5 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupNotificationBar();
    qint64 t6 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupDock();
    qint64 t7 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupToolBar();
    qint64 t8 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupDataTable();
    qint64 t9 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupWindowManager();
    qint64 t10 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    setupMinimap();
    qint64 t11 = QDateTime::currentDateTime().toMSecsSinceEpoch();

    qCritical() << "setupWelcomeScreen in: " <<  t2 - t1 << "MS";
    qCritical() << "setupMenuBar in: " <<  t3 - t2 << "MS";
    qCritical() << "setupSearchBar in: " <<  t4 - t3 << "MS";
    qCritical() << "setupProgressBar in: " <<  t5 - t4 << "MS";
    qCritical() << "setupNotificationBar in: " <<  t6 - t5 << "MS";
    qCritical() << "setupDock in: " <<  t7 - t6 << "MS";
    qCritical() << "setupToolBar in: " <<  t8 - t7 << "MS";
    qCritical() << "setupDataTable in: " <<  t9 - t8 << "MS";
    qCritical() << "setupWindowManager in: " <<  t10 - t9 << "MS";
    qCritical() << "setupMinimap in: " <<  t11 - t10 << "MS";
}


/**
 * @brief MedeaMainWindow::setupInnerWindow
 */
void MainWindow::setupInnerWindow()
{
    innerWindow = WindowManager::constructCentralWindow("Main Window");
    setCentralWidget(innerWindow);

    NodeView* nodeView_Interfaces = new NodeView();
    NodeView* nodeView_Behaviour = new NodeView();
    NodeView* nodeView_Assemblies = new NodeView();
    NodeView* nodeView_Hardware = new NodeView();
    QOSBrowser* qosBrowser = new QOSBrowser(viewController, this);

    nodeView_Interfaces->setContainedViewAspect(VA_INTERFACES);
    nodeView_Behaviour->setContainedViewAspect(VA_BEHAVIOUR);
    nodeView_Assemblies->setContainedViewAspect(VA_ASSEMBLIES);
    nodeView_Hardware->setContainedViewAspect(VA_HARDWARE);

    BaseDockWidget *dwInterfaces = WindowManager::constructNodeViewDockWidget("Interface", Qt::TopDockWidgetArea);
    dwInterfaces->setWidget(nodeView_Interfaces);
    dwInterfaces->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dwInterfaces->setIcon("Items", "InterfaceDefinitions");
    dwInterfaces->setIconVisible(false);

    BaseDockWidget *dwBehaviour = WindowManager::constructNodeViewDockWidget("Behaviour", Qt::TopDockWidgetArea);
    dwBehaviour->setWidget(nodeView_Behaviour);
    dwBehaviour->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dwBehaviour->setIcon("Items", "BehaviourDefinitions");
    dwBehaviour->setIconVisible(false);

    BaseDockWidget *dwAssemblies = WindowManager::constructNodeViewDockWidget("Assemblies", Qt::BottomDockWidgetArea);
    dwAssemblies->setWidget(nodeView_Assemblies);
    dwAssemblies->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dwAssemblies->setIcon("Items", "AssemblyDefinitions");
    dwAssemblies->setIconVisible(false);

    BaseDockWidget *dwHardware = WindowManager::constructNodeViewDockWidget("Hardware", Qt::BottomDockWidgetArea);
    dwHardware->setWidget(nodeView_Hardware);
    dwHardware->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dwHardware->setIcon("Items", "HardwareDefinitions");
    dwHardware->setIconVisible(false);

    BaseDockWidget *qosDockWidget = WindowManager::constructViewDockWidget("QOS Browser");
    qosDockWidget->setWidget(qosBrowser);
    qosDockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    qosDockWidget->setIcon("Items", "QOSProfile");

    dwInterfaces->setProtected(true);
    dwBehaviour->setProtected(true);
    dwAssemblies->setProtected(true);
    dwHardware->setProtected(true);
    qosDockWidget->setProtected(true);

    SettingsController* s = SettingsController::settings();

    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dwInterfaces);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dwBehaviour);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dwAssemblies);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dwHardware);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, qosDockWidget);

    innerWindow->setDockWidgetVisibility(dwInterfaces,   s->getSetting(SK_WINDOW_INTERFACES_VISIBLE).toBool());
    innerWindow->setDockWidgetVisibility(dwBehaviour,    s->getSetting(SK_WINDOW_BEHAVIOUR_VISIBLE).toBool());
    innerWindow->setDockWidgetVisibility(dwAssemblies,   s->getSetting(SK_WINDOW_ASSEMBLIES_VISIBLE).toBool());
    innerWindow->setDockWidgetVisibility(dwHardware,     s->getSetting(SK_WINDOW_HARDWARE_VISIBLE).toBool());
    innerWindow->setDockWidgetVisibility(qosDockWidget,  s->getSetting(SK_WINDOW_QOS_VISIBLE).toBool());

    // NOTE: Apparently calling innerWindow's createPopupMenu crashes the
    // application if it's called before the dock widgets are added above
    // This function needs to be called after the code above and before the connections below
    setupMainDockWidgetToggles();

    connectNodeView(nodeView_Interfaces);
    connectNodeView(nodeView_Behaviour);
    connectNodeView(nodeView_Assemblies);
    connectNodeView(nodeView_Hardware);

    // connect aspect toggle buttons
    connect(dwInterfaces, SIGNAL(visibilityChanged(bool)), interfaceButton, SLOT(setChecked(bool)));
    connect(dwBehaviour, SIGNAL(visibilityChanged(bool)), behaviourButton, SLOT(setChecked(bool)));
    connect(dwAssemblies, SIGNAL(visibilityChanged(bool)), assemblyButton, SLOT(setChecked(bool)));
    connect(dwHardware, SIGNAL(visibilityChanged(bool)), hardwareButton, SLOT(setChecked(bool)));
    connect(interfaceButton, SIGNAL(clicked(bool)), dwInterfaces, SLOT(setVisible(bool)));
    connect(behaviourButton, SIGNAL(clicked(bool)), dwBehaviour, SLOT(setVisible(bool)));
    connect(assemblyButton, SIGNAL(clicked(bool)), dwAssemblies, SLOT(setVisible(bool)));
    connect(hardwareButton, SIGNAL(clicked(bool)), dwHardware, SLOT(setVisible(bool)));
    connect(restoreAspectsButton, SIGNAL(clicked(bool)), innerWindow, SLOT(resetDockWidgets()));
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

    //applicationToolbar->addWidget(w1);
    applicationToolbar->addActions(viewController->getActionController()->applicationToolbar->actions());
    //applicationToolbar->addWidget(w2);

    BaseDockWidget* dockWidget = WindowManager::constructToolDockWidget("Toolbar");
    //dockWidget->setTitleBarWidget(applicationToolbar);
    dockWidget->setTitleBarWidget(frame);
    dockWidget->setAllowedAreas(Qt::TopDockWidgetArea);
    //dockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::LeftDockWidgetArea | Qt::BottomDockWidgetArea);

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

    searchDialog = new SearchDialog(this);

    connect(this, &MainWindow::requestSuggestions, viewController, &ViewController::requestSearchSuggestions);
    connect(viewController, &ViewController::vc_gotSearchSuggestions, this, &MainWindow::updateSearchSuggestions);
    connect(viewController, &ViewController::vc_setupModel, searchDialog, &SearchDialog::resetDialog);

    connect(searchBar, SIGNAL(returnPressed()), searchButton, SLOT(click()));
    connect(searchButton, SIGNAL(clicked(bool)), searchPopup, SLOT(hide()));
    connect(searchButton, SIGNAL(clicked(bool)), this, SLOT(searchEntered()));
    connect(searchDialog, SIGNAL(centerOnViewItem(int)), viewController, SLOT(centerOnID(int)));
    connect(searchDialog, SIGNAL(popupViewItem(int)), viewController, SLOT(popupItem(int)));
    connect(searchDialog, SIGNAL(itemHoverEnter(int)), viewController->getToolbarController(), SLOT(actionHoverEnter(int)));
    connect(searchDialog, SIGNAL(itemHoverLeave(int)), viewController->getToolbarController(), SLOT(actionHoverLeave(int)));
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
    notificationTimer = new QTimer(this);

    notificationIconLabel = new QLabel(this);
    notificationIconLabel->setAlignment(Qt::AlignCenter);

    notificationLabel = new QLabel("This is a notification.", this);
    notificationLabel->setFont(QFont(font().family(), 11));
    notificationLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    //notificationLabel->setWordWrap(true);

    notificationWidget = new QWidget(this);
    notificationWidget->setContentsMargins(5, 2, 5, 2);

    QHBoxLayout* layout = new QHBoxLayout(notificationWidget);
    layout->setMargin(0);
    layout->setSpacing(5);
    layout->addWidget(notificationIconLabel, 0, Qt::AlignCenter);
    layout->addWidget(notificationLabel, 1, Qt::AlignCenter);

    notificationPopup = new PopupWidget(PopupWidget::TOOL, this);
    notificationPopup->setWidget(notificationWidget);
    notificationPopup->hide();

    notificationDialog = new NotificationDialog(this);

    connect(notificationDialog, &NotificationDialog::notificationAdded, viewController, &ViewController::notificationAdded);
    connect(notificationDialog, &NotificationDialog::centerOn, viewController, &ViewController::centerOnID);
    connect(viewController, &ViewController::vc_setupModel, viewController, &ViewController::notificationsSeen);
    connect(viewController, &ViewController::vc_setupModel, notificationDialog, &NotificationDialog::resetDialog);
    connect(viewController->getActionController()->window_showNotifications, &QAction::triggered, notificationDialog, &NotificationDialog::toggleVisibility);
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
    tableWidget = new DataTableWidget(viewController, this);

    BaseDockWidget* dockWidget = WindowManager::constructToolDockWidget("Table");
    dockWidget->setWidget(tableWidget);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);

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
 * @brief MedeaMainWindow::setupWindowManager
 */
void MainWindow::setupWindowManager()
{
    viewManager = WindowManager::manager()->getViewManagerGUI();
    viewManager->setMinimumHeight(210);

    BaseDockWidget* dockWidget = WindowManager::constructToolDockWidget("View Manager");
    dockWidget->setWidget(viewManager);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    addDockWidget(Qt::RightDockWidgetArea, dockWidget, Qt::Vertical);
    setDockWidgetVisibility(dockWidget, SettingsController::settings()->getSetting(SK_WINDOW_BROWSER_VISIBLE).toBool());
}


/**
 * @brief MedeaMainWindow::setupMainDockWidgetToggles
 * NOTE: This neeeds to be called after the tool dock widgets
 * and both the central and inner windows are constructed.
 */
void MainWindow::setupMainDockWidgetToggles()
{
    interfaceButton = new QToolButton(this);
    behaviourButton = new QToolButton(this);
    assemblyButton = new QToolButton(this);
    hardwareButton = new QToolButton(this);
    restoreAspectsButton = new QToolButton(this);
    restoreToolsButton = new QToolButton(this);

    /*
    interfaceButton->setText("I");
    behaviourButton->setText("B");
    assemblyButton->setText("A");
    hardwareButton->setText("H");
    */

    interfaceButton->setToolTip("Toggle Interface Aspect");
    behaviourButton->setToolTip("Toggle Behaviour Aspect");
    assemblyButton->setToolTip("Toggle Assembly Aspect");
    hardwareButton->setToolTip("Toggle Hardware Aspect");
    restoreAspectsButton->setToolTip("Restore Main Dock Widgets");
    restoreToolsButton->setToolTip("Restore Tool Dock Widgets");

    restoreAspectsButton->hide();

    interfaceButton->setCheckable(true);
    behaviourButton->setCheckable(true);
    assemblyButton->setCheckable(true);
    hardwareButton->setCheckable(true);

    QMenu* menu = createPopupMenu();
    restoreToolsAction = menu->addAction("Show All Tool Widgets");
    restoreToolsButton->setMenu(menu);
    restoreToolsButton->setPopupMode(QToolButton::InstantPopup);

    if (innerWindow) {
        restoreAspectsButton->setMenu(innerWindow->createPopupMenu());
        restoreAspectsButton->setPopupMode(QToolButton::InstantPopup);
    }

    QToolBar* toolbar = new QToolBar(this);
    toolbar->setIconSize(QSize(20,20));
    toolbar->setFixedHeight(menuBar->height() - 6);
    toolbar->setStyleSheet("QToolButton{ padding: 2px 4px; }");

    toolbar->addAction(viewController->getActionController()->window_showNotifications);
    toolbar->addSeparator();
    toolbar->addWidget(interfaceButton);
    toolbar->addWidget(behaviourButton);
    toolbar->addWidget(assemblyButton);
    toolbar->addWidget(hardwareButton);
    toolbar->addSeparator();
    toolbar->addWidget(restoreAspectsButton);
    toolbar->addWidget(restoreToolsButton);

    menuBar->setCornerWidget(toolbar);

    connect(restoreToolsAction, SIGNAL(triggered(bool)), this, SLOT(resetToolDockWidgets()));
}


/**
 * @brief MedeaMainWindow::setupJenkinsManager
 */
void MainWindow::setupJenkinsManager()
{
    if(!jenkinsManager){
        jenkinsManager = new JenkinsManager(this);
        connect(jenkinsManager, &JenkinsManager::settingsValidationComplete, viewController, &ViewController::jenkinsManager_SettingsValidated);

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
    minimap->parentWidget()->resize(defaultWidth, windowHeight/4);
    viewManager->parentWidget()->resize(defaultWidth, windowHeight/4);
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
        cw = WindowManager::manager()->getActiveWindow();
        widgetPos.ry() -= widget->height()/2 + 8;
    }
    if (cw == innerWindow) {
        widgetPos = pos();
    }
    if (cw && widget) {
        widgetPos += cw->geometry().center();
        switch (alignment) {
        case Qt::AlignBottom:
            widgetPos.ry() += cw->height() / 2;
            break;
        default:
            break;
        }
        widgetPos -= QPointF(widget->width()/2, widget->height()/2);
        widget->move(widgetPos.x(), widgetPos.y());
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
