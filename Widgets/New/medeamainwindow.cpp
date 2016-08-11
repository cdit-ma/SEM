#include "medeamainwindow.h"
#include "medeaviewdockwidget.h"
#include "medeatooldockwidget.h"
#include "../../View/theme.h"
#include "selectioncontroller.h"
#include "medeanodeviewdockwidget.h"
#include "../../Controller/settingscontroller.h"

#include <QDebug>
#include <QHeaderView>
#include <QPushButton>
#include <QMenuBar>
#include <QDateTime>
#include <QApplication>
#include <QStringBuilder>
#include "../../Controller/settingscontroller.h"
#define TOOLBAR_HEIGHT 32

MedeaMainWindow::MedeaMainWindow(ViewController *vc, QWidget* parent):MedeaWindowNew(parent, MedeaWindowNew::MAIN_WINDOW)
{
    qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
    SettingsController::initializeSettings();
    connect(SettingsController::settings(), SIGNAL(settingChanged(SETTING_KEY,QVariant)), this, SLOT(settingChanged(SETTING_KEY,QVariant)));

    initializeApplication();

    floatingToolbar = 0;
    viewController = vc;

    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    resize(1000, 600);

    setupTools();
    setupInnerWindow();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(MedeaWindowManager::manager(), SIGNAL(activeViewDockWidgetChanged(MedeaViewDockWidget*,MedeaViewDockWidget*)), this, SLOT(activeViewDockWidgetChanged(MedeaViewDockWidget*, MedeaViewDockWidget*)));
    setViewController(vc);

    themeChanged();
    qint64 time2 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    show();
    qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();


    qCritical() << "MedeaMainWindow in: " <<  time2 - timeStart << "MS";
    qCritical() << "MedeaMainWindow->show() in: " <<  timeFinish - time2 << "MS";
    setModelTitle("");
}

MedeaMainWindow::~MedeaMainWindow()
{
    qCritical() << "~MedeaMainWindow()";
    SettingsController::teardownSettings();
}

void MedeaMainWindow::setViewController(ViewController *vc)
{
    viewController = vc;
    SelectionController* controller = vc->getSelectionController();

    connect(controller, SIGNAL(itemActiveSelectionChanged(ViewItem*,bool)), tableWidget, SLOT(itemActiveSelectionChanged(ViewItem*, bool)));
    connect(vc->getActionController()->view_viewInNewWindow, SIGNAL(triggered(bool)), this, SLOT(spawnSubView()));

    connect(vc, &ViewController::projectModified, this, &MedeaMainWindow::setWindowModified);

    connect(vc, &ViewController::projectPathChanged, this, &MedeaMainWindow::setModelTitle);

    //this->addToolBar(Qt::BottomToolBarArea, viewController->getToolbarController()->toolbar);

    if (vc->getActionController()) {
        connect(vc->getActionController()->getRootAction("Root_Search"), SIGNAL(triggered(bool)), this, SLOT(popupSearch()));
    }
}

void MedeaMainWindow::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet(theme->getWindowStyleSheet() +
                  theme->getViewStyleSheet() +
                  theme->getMenuBarStyleSheet() +
                  theme->getMenuStyleSheet() +
                  theme->getToolBarStyleSheet() +
                  theme->getDockWidgetStyleSheet() +
                  theme->getPushButtonStyleSheet() +
                  theme->getPopupWidgetStyleSheet() +
                  theme->getTabbedWidgetStyleSheet() +
                  theme->getScrollBarStyleSheet() +
                  "QToolButton{ padding: 4px; }");

    innerWindow->setStyleSheet(theme->getWindowStyleSheet());

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

    searchBar->setStyleSheet(theme->getLineEditStyleSheet());
    searchButton->setIcon(theme->getIcon("Actions", "Search"));
    searchOptionsButton->setIcon(theme->getIcon("Actions", "Settings"));
    popupSearchButton->setIcon(theme->getIcon("Actions", "Search"));

    restoreAspectsButton->setIcon(theme->getIcon("Actions", "MenuView"));
    restoreToolsButton->setIcon(theme->getIcon("Actions", "Build"));

    interfaceButton->setStyleSheet(theme->getAspectButtonStyleSheet(VA_INTERFACES));
    behaviourButton->setStyleSheet(theme->getAspectButtonStyleSheet(VA_BEHAVIOUR));
    assemblyButton->setStyleSheet(theme->getAspectButtonStyleSheet(VA_ASSEMBLIES));
    hardwareButton->setStyleSheet(theme->getAspectButtonStyleSheet(VA_HARDWARE));
}


void MedeaMainWindow::activeViewDockWidgetChanged(MedeaViewDockWidget *viewDock, MedeaViewDockWidget *prevDock)
{
    if(viewDock && viewDock->isNodeViewDock()){
        MedeaNodeViewDockWidget* nodeViewDock = (MedeaNodeViewDockWidget*) viewDock;
        NodeViewNew* view = nodeViewDock->getNodeView();

        if(prevDock && prevDock->isNodeViewDock()){
            MedeaNodeViewDockWidget* prevNodeViewDock = (MedeaNodeViewDockWidget*) prevDock;
            NodeViewNew* prevView = prevNodeViewDock->getNodeView();
            if(prevView){
                disconnect(minimap, SIGNAL(minimap_Pan(QPointF)), prevView, SLOT(minimap_Pan(QPointF)));
                disconnect(minimap, SIGNAL(minimap_Panning(bool)), prevView, SLOT(minimap_Panning(bool)));
                disconnect(minimap, SIGNAL(minimap_Zoom(int)), prevView, SLOT(minimap_Zoom(int)));
                disconnect(prevView, SIGNAL(viewportChanged(QRectF, qreal)), minimap, SLOT(viewportRectChanged(QRectF, qreal)));
            }
        }

        if(view){
            minimap->setBackgroundColor(view->getBackgroundColor());
            minimap->setScene(view->scene());

            connect(minimap, SIGNAL(minimap_Pan(QPointF)), view, SLOT(minimap_Pan(QPointF)));
            connect(minimap, SIGNAL(minimap_Panning(bool)), view, SLOT(minimap_Panning(bool)));
            connect(minimap, SIGNAL(minimap_Zoom(int)), view, SLOT(minimap_Zoom(int)));
            connect(view, SIGNAL(viewportChanged(QRectF, qreal)), minimap, SLOT(viewportRectChanged(QRectF, qreal)));

            view->viewportChanged();
        }
    }
}

void MedeaMainWindow::spawnSubView()
{
    if(viewController){
        SelectionController* selectionController = viewController->getSelectionController();

        QVector<ViewItem*> items = selectionController->getSelection();

        if(items.length() == 1){

            ViewItem* item = items.first();
            if(item->isNode()){
                MedeaDockWidget *dockWidget = MedeaWindowManager::constructNodeViewDockWidget("SubView", Qt::TopDockWidgetArea);
                dockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
                dockWidget->setParent(this);
                dockWidget->setIcon(item->getIcon());
                dockWidget->setTitle(item->getData("label").toString());
                innerWindow->addDockWidget(Qt::TopDockWidgetArea, dockWidget);

                NodeViewNew* nodeView = new NodeViewNew(dockWidget);
                nodeView->setContainedNodeViewItem((NodeViewItem*)item);
                connectNodeView(nodeView);
                dockWidget->setWidget(nodeView);
                nodeView->fitToScreen();
            }
        }
    }
}

void MedeaMainWindow::popupSearch()
{
    if (searchBar->isVisible()) {
        searchBar->setFocus();
    } else {
        QPointF centralWidgetCenter = pos() + innerWindow->pos() + innerWindow->rect().center();
        centralWidgetCenter -= QPointF(searchToolbar->width()/2, searchToolbar->height()/2);
        searchToolbar->move(centralWidgetCenter.x(), centralWidgetCenter.y());
        searchToolbar->show();
        popupSearchBar->setFocus();
    }
}

void MedeaMainWindow::toolbarChanged(Qt::DockWidgetArea area)
{
    if(area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea){
        floatingToolbar->setOrientation(Qt::Horizontal);
        floatingToolbar->setFixedHeight(QWIDGETSIZE_MAX);
        floatingToolbar->setFixedWidth(QWIDGETSIZE_MAX);
    }else{
        floatingToolbar->setOrientation(Qt::Vertical);
        resizeEvent(0);
    }
}

void MedeaMainWindow::toolbarTopLevelChanged(bool undocked)
{
    if(undocked){
        if(floatingToolbar->orientation() == Qt::Vertical){
            floatingToolbar->setOrientation(Qt::Horizontal);
            floatingToolbar->setFixedHeight(QWIDGETSIZE_MAX);
        }
        floatingToolbar->parentWidget()->resize(floatingToolbar->sizeHint() +  QSize(12,0));
    }
}

void MedeaMainWindow::setModelTitle(QString modelTitle)
{
    if(!modelTitle.isEmpty()){
        modelTitle = "- " % modelTitle;
    }
    QString title = "MEDEA " % modelTitle % "[*]";
    setWindowTitle(title);
}

void MedeaMainWindow::settingChanged(SETTING_KEY setting, QVariant value)
{
    /*
    bool boolValue = value.toBool();
    //Handle stuff.
    switch(setting){
case SK_WINDOW_INTERFACES_VISIBLE:{
        nodeView_Interfaces->parentWidget()->setVisible(boolValue);
        break;
    }
case SK_WINDOW_BEHAVIOUR_VISIBLE:{
        nodeView_Behaviour->parentWidget()->setVisible(boolValue);
        break;
    }
case SK_WINDOW_ASSEMBLIES_VISIBLE:{
        nodeView_Assemblies->parentWidget()->setVisible(boolValue);
        break;
    }
case SK_WINDOW_HARDWARE_VISIBLE:{
        nodeView_Hardware->parentWidget()->setVisible(boolValue);
        break;
    }
case SK_WINDOW_TABLE_VISIBLE:
case SK_WINDOW_MINIMAP_VISIBLE:
case SK_WINDOW_BROWSER_VISIBLE:
case SK_WINDOW_TOOLBAR_VISIBLE:
    default:
        break;
}*/
}


void MedeaMainWindow::initializeApplication()
{
    //Allow Drops
    setAcceptDrops(true);

    //Set QApplication information.
    QApplication::setApplicationName("MEDEA");
    QApplication::setApplicationVersion(APP_VERSION);
    QApplication::setOrganizationName("Defence Information Group");
    QApplication::setOrganizationDomain("http://blogs.adelaide.edu.au/dig/");
    QApplication::setWindowIcon(Theme::theme()->getIcon("Actions", "MEDEA"));

    //Set Font.
    int opensans_FontID = QFontDatabase::addApplicationFont(":/Resources/Fonts/OpenSans-Regular.ttf");
    QString opensans_fontname = QFontDatabase::applicationFontFamilies(opensans_FontID).at(0);
    //QFont font = QFont(opensans_fontname);
    QFont font("Verdana");
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPointSizeF(8.5);
    QApplication::setFont(font);
}


void MedeaMainWindow::connectNodeView(NodeViewNew *nodeView)
{
    if(nodeView && viewController){
        nodeView->setViewController(viewController);
        connect(viewController, SIGNAL(viewItemConstructed(ViewItem*)), nodeView, SLOT(viewItem_Constructed(ViewItem*)));
        connect(viewController, SIGNAL(viewItemDestructing(int,ViewItem*)), nodeView, SLOT(viewItem_Destructed(int,ViewItem*)));
    }
}

void MedeaMainWindow::setupTools()
{
    //setupMenuAndTitle();
    setupMenuBar();
    setupToolBar();
    setupSearchBar();
    setupPopupSearchBar();
    setupDataTable();
    setupMinimap();
    setupMainDockWidgetToggles();
}

void MedeaMainWindow::setupInnerWindow()
{
    innerWindow = MedeaWindowManager::constructCentralWindow();

    setCentralWidget(innerWindow);

    nodeView_Interfaces = new NodeViewNew();
    nodeView_Behaviour = new NodeViewNew();
    nodeView_Assemblies = new NodeViewNew();
    nodeView_Hardware = new NodeViewNew();

    nodeView_Interfaces->setContainedViewAspect(VA_INTERFACES);
    nodeView_Behaviour->setContainedViewAspect(VA_BEHAVIOUR);
    nodeView_Assemblies->setContainedViewAspect(VA_ASSEMBLIES);
    nodeView_Hardware->setContainedViewAspect(VA_HARDWARE);

    MedeaDockWidget *dwInterfaces = MedeaWindowManager::constructNodeViewDockWidget("Interface", Qt::TopDockWidgetArea);
    dwInterfaces->setWidget(nodeView_Interfaces);
    dwInterfaces->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    MedeaDockWidget *dwBehaviour = MedeaWindowManager::constructNodeViewDockWidget("Behaviour", Qt::TopDockWidgetArea);
    dwBehaviour->setWidget(nodeView_Behaviour);
    dwBehaviour->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    MedeaDockWidget *dwAssemblies = MedeaWindowManager::constructNodeViewDockWidget("Assemblies", Qt::BottomDockWidgetArea);
    dwAssemblies->setWidget(nodeView_Assemblies);
    dwAssemblies->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    MedeaDockWidget *dwHardware = MedeaWindowManager::constructNodeViewDockWidget("Hardware", Qt::BottomDockWidgetArea);
    dwHardware->setWidget(nodeView_Hardware);
    dwHardware->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    dwInterfaces->setProtected(true);
    dwBehaviour->setProtected(true);
    dwAssemblies->setProtected(true);
    dwHardware->setProtected(true);

    //dwInterfaces->setIcon("Items", "InterfaceDefinitions");
    //dwBehaviour->setIcon("Items", "BehaviourDefinitions");
    //dwAssemblies->setIcon("Items", "AssemblyDefinitions");
    //dwHardware->setIcon("Items", "HardwareDefinitions");

    SettingsController* settings = SettingsController::settings();
    dwInterfaces->setVisible(settings->getSetting(SK_WINDOW_INTERFACES_VISIBLE).toBool());
    dwBehaviour->setVisible(settings->getSetting(SK_WINDOW_BEHAVIOUR_VISIBLE).toBool());
    dwAssemblies->setVisible(settings->getSetting(SK_WINDOW_ASSEMBLIES_VISIBLE).toBool());
    dwHardware->setVisible(settings->getSetting(SK_WINDOW_HARDWARE_VISIBLE).toBool());

    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dwInterfaces);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dwBehaviour);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dwAssemblies);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dwHardware);

    connectNodeView(nodeView_Interfaces);
    connectNodeView(nodeView_Behaviour);
    connectNodeView(nodeView_Assemblies);
    connectNodeView(nodeView_Hardware);

    MedeaDockWidget *qosDockWidget = MedeaWindowManager::constructViewDockWidget("QOS Browser");
    qosBrowser = new QOSBrowser(viewController, this);
    qosDockWidget->setWidget(qosBrowser);
    qosDockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    qosDockWidget->setProtected(true);
    qosDockWidget->setVisible(false);

    innerWindow->addDockWidget(Qt::TopDockWidgetArea, qosDockWidget);

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

void MedeaMainWindow::setupMenuAndTitle()
{
    menuButton = new QPushButton(Theme::theme()->getIcon("Actions", "MEDEAIcon"), "", this);
    menuButton->setFixedSize(30,30);
    menuButton->setIconSize(menuButton->size() - QSize(2,8));

    projectTitleButton = new QPushButton("Project Title", this);
    projectTitleButton->setFlat(true);
    projectTitleButton->setFont(QFont("Verdana", 10));
    projectTitleButton->setStyleSheet("QPushButton{ border: none; border-radius: 2px; }"
                                      "QPushButton:!hover{ background: rgba(0,0,0,0); }");

    middlewareButton = new QToolButton(this);
    middlewareButton->setFixedSize(20,20);
    middlewareButton->setToolTip("Select Model's Middleware");

    closeProjectButton = new QToolButton(this);
    closeProjectButton->setFixedSize(20,20);
    closeProjectButton->setToolTip("Close Active Project");

    QWidget* spacerWidget1 = new QWidget(this);
    QWidget* spacerWidget2 = new QWidget(this);
    spacerWidget1->setFixedSize(5, 1);
    spacerWidget2->setFixedSize(5, 1);

    QToolBar* toolbar = new QToolBar(this);
    toolbar->addWidget(menuButton);
    toolbar->addWidget(spacerWidget1);
    toolbar->addWidget(projectTitleButton);
    toolbar->addWidget(spacerWidget2);
    toolbar->addWidget(middlewareButton);
    toolbar->addWidget(closeProjectButton);

    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setFixedHeight(TOOLBAR_HEIGHT);
    toolbar->setMinimumWidth(toolbar->sizeHint().width());
    addToolBar(Qt::TopToolBarArea, toolbar);

    //menuButton->setMenu(viewController->getActionController()->mainMenu);
}

void MedeaMainWindow::setupMenuBar()
{
    menuBar = new QMenuBar(this);
    menuBar->addMenu(viewController->getActionController()->menu_file);
    menuBar->addMenu(viewController->getActionController()->menu_edit);
    menuBar->addMenu(viewController->getActionController()->menu_view);
    menuBar->addMenu(viewController->getActionController()->menu_model);
    menuBar->addMenu(viewController->getActionController()->menu_jenkins);
    menuBar->addMenu(viewController->getActionController()->menu_window);
    menuBar->addMenu(viewController->getActionController()->menu_options);
    menuBar->addMenu(viewController->getActionController()->menu_help);

    // TODO - Find out how to set the height of the menubar items
    menuBar->setFixedHeight(TOOLBAR_HEIGHT);
    menuBar->setNativeMenuBar(false);
    setMenuBar(menuBar);
}

void MedeaMainWindow::setupToolBar()
{
    floatingToolbar = new QToolBar(this);
    floatingToolbar->setMovable(false);
    floatingToolbar->setFloatable(false);
    floatingToolbar->setIconSize(QSize(20,20));

    QWidget* w1 = new QWidget(this);
    QWidget* w2 = new QWidget(this);
    w1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    w2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    floatingToolbar->addWidget(w1);
    floatingToolbar->addActions(viewController->getActionController()->applicationToolbar->actions());
    floatingToolbar->addWidget(w2);

    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("Toolbar");
    dockWidget->setTitleBarWidget(floatingToolbar);
    dockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::LeftDockWidgetArea | Qt::BottomDockWidgetArea);

    connect(dockWidget, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(toolbarChanged(Qt::DockWidgetArea)));
    connect(dockWidget, SIGNAL(topLevelChanged(bool)), this, SLOT(toolbarTopLevelChanged(bool)));

    //Check visibility state.
    dockWidget->setVisible(SettingsController::settings()->getSetting(SK_WINDOW_TOOLBAR_VISIBLE).toBool());

    addDockWidget(Qt::TopDockWidgetArea, dockWidget, Qt::Horizontal);
}

void MedeaMainWindow::setupSearchBar()
{   
    searchBar = new QLineEdit(this);
    searchBar->setFixedWidth(183);
    searchBar->setPlaceholderText("Search Here...");
    searchBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    searchButton = new QToolButton(this);
    searchButton->setToolTip("Submit Search");

    searchOptionsButton = new QToolButton(this);
    searchOptionsButton->setToolTip("Search Settings");

    QToolBar* toolbar = new QToolBar(this);
    toolbar->setIconSize(QSize(18,18));
    toolbar->setFixedHeight(menuBar->height() - 6);
    toolbar->addWidget(searchBar);
    toolbar->addWidget(searchButton);
    toolbar->addWidget(searchOptionsButton);

    toolbar->hide();
    //menuBar->setCornerWidget(toolbar);
}

void MedeaMainWindow::setupPopupSearchBar()
{
    popupSearchButton = new QToolButton(this);
    popupSearchButton->setToolTip("Submit Search");

    popupSearchBar = new QLineEdit(this);
    popupSearchBar->setFont(QFont(font().family(), 13));
    popupSearchBar->setPlaceholderText("Search Here...");
    popupSearchBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    int toolbarWidth = 300;
    searchToolbar = new QToolBar(this);
    searchToolbar->setIconSize(QSize(24,24));
    searchToolbar->setFixedSize(toolbarWidth, 45);
    searchToolbar->setMovable(false);
    searchToolbar->setFloatable(false);
    searchToolbar->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    searchToolbar->setAttribute(Qt::WA_NoSystemBackground, true);
    searchToolbar->setAttribute(Qt::WA_TranslucentBackground, true);
    searchToolbar->setObjectName("POPUP_WIDGET");

    searchToolbar->addWidget(popupSearchBar);
    searchToolbar->addWidget(popupSearchButton);
    searchToolbar->hide();

    searchSuggestions = new SearchSuggestCompletion(popupSearchBar);
    searchSuggestions->setSize(toolbarWidth, 0, SearchSuggestCompletion::ST_MIN);
    searchSuggestions->setSize(toolbarWidth, 0, SearchSuggestCompletion::ST_MAX);

    connect(viewController, SIGNAL(seachSuggestions(QStringList)), searchSuggestions, SLOT(showCompletion(QStringList)));
    connect(popupSearchBar, SIGNAL(textChanged(QString)), viewController, SLOT(searchSuggestionsRequested(QString)));
    connect(popupSearchBar, SIGNAL(returnPressed()), popupSearchButton, SLOT(click()));
    connect(popupSearchButton, SIGNAL(clicked(bool)), searchToolbar, SLOT(hide()));
}

void MedeaMainWindow::setupDataTable()
{
    tableWidget = new TableWidget(viewController, this);

    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("Table");
    dockWidget->setWidget(tableWidget);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    //Check visibility state.
    dockWidget->setVisible(SettingsController::settings()->getSetting(SK_WINDOW_TABLE_VISIBLE).toBool());
    addDockWidget(Qt::RightDockWidgetArea, dockWidget, Qt::Vertical);
}

void MedeaMainWindow::setupMinimap()
{
    minimap = new NodeViewMinimap(this);
    minimap->setFixedHeight(175);

    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("Minimap");
    dockWidget->setWidget(minimap);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    //Check visibility state.
    dockWidget->setVisible(SettingsController::settings()->getSetting(SK_WINDOW_MINIMAP_VISIBLE).toBool());
    addDockWidget(Qt::RightDockWidgetArea, dockWidget, Qt::Vertical);
}

void MedeaMainWindow::setupMainDockWidgetToggles()
{
    interfaceButton = new QToolButton(this);
    behaviourButton = new QToolButton(this);
    assemblyButton = new QToolButton(this);
    hardwareButton = new QToolButton(this);
    qosBrowserButton = new QToolButton(this);
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
    restoreAspectsButton->setToolTip("Restore All Aspects");
    restoreToolsButton->setToolTip("Restore All Tools");

    interfaceButton->setCheckable(true);
    behaviourButton->setCheckable(true);
    assemblyButton->setCheckable(true);
    hardwareButton->setCheckable(true);

    QToolBar* toolbar = new QToolBar(this);
    toolbar->setIconSize(QSize(20,20));
    toolbar->setFixedHeight(menuBar->height() - 6);
    toolbar->setStyleSheet("QToolButton{ padding: 2px 4px; }");

    qosBrowserButton->hide();

    //toolbar->addWidget(qosBrowserButton);
    //toolbar->addSeparator();
    toolbar->addWidget(interfaceButton);
    toolbar->addWidget(behaviourButton);
    toolbar->addWidget(assemblyButton);
    toolbar->addWidget(hardwareButton);
    toolbar->addSeparator();
    toolbar->addWidget(restoreAspectsButton);
    toolbar->addWidget(restoreToolsButton);

    menuBar->setCornerWidget(toolbar);
}

void MedeaMainWindow::resizeEvent(QResizeEvent *e)
{
    if(e){
        QMainWindow::resizeEvent(e);
    }
    if(floatingToolbar && floatingToolbar->orientation() == Qt::Vertical){
        floatingToolbar->setFixedHeight(centralWidget()->rect().height());
    }
}
