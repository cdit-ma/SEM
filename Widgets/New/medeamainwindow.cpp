#include "medeamainwindow.h"
#include "medeaviewdockwidget.h"
#include "medeatooldockwidget.h"
#include "../../View/theme.h"
#include "selectioncontroller.h"
#include "medeanodeviewdockwidget.h"


#include <QDebug>
#include <QHeaderView>
#include <QPushButton>
#include <QMenuBar>
#include <QDateTime>
#include <QApplication>
#include "../../Controller/settingscontroller.h"
#define TOOLBAR_HEIGHT 32


MedeaMainWindow::MedeaMainWindow(ViewController *vc, QWidget* parent):MedeaWindowNew(parent, MedeaWindowNew::MAIN_WINDOW)
{
    qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();

    initializeApplication();
    initializeSettings();
    initializeTheme();

    floatingToolbar = 0;
    viewController = vc;

    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    setMinimumSize(1000,600);

    setupInnerWindow();
    setupTools();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(MedeaWindowManager::manager(), SIGNAL(activeViewDockWidgetChanged(MedeaViewDockWidget*,MedeaViewDockWidget*)), this, SLOT(activeViewDockWidgetChanged(MedeaViewDockWidget*, MedeaViewDockWidget*)));
    setViewController(vc);
    showNormal();

    emit Theme::theme()->theme_Changed();
    qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();
    qCritical() << "MedeaMainWindow in: " <<  timeFinish - timeStart << "MS";
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
                  "QToolButton{ padding: 4px; }");

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
    popupSearchBar->setStyleSheet(theme->getLineEditStyleSheet());
    popupSearchButton->setIcon(theme->getIcon("Actions", "Search"));

    interfaceButton->setIcon(theme->getIcon("Items", "InterfaceDefinitions"));
    behaviourButton->setIcon(theme->getIcon("Items", "BehaviourDefinitions"));
    assemblyButton->setIcon(theme->getIcon("Items", "AssemblyDefinitions"));
    hardwareButton->setIcon(theme->getIcon("Items", "HardwareDefinitions"));
    //interfaceButton->setIcon(theme->getIcon("Items", "InterfaceDefinitions"));
    //interfaceButton->setIcon(theme->getIcon("Items", "InterfaceDefinitions"));
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
            NodeViewNew* nodeView = new NodeViewNew();
            connectNodeView(nodeView);
            ViewItem* item = items.first();
            if(item->isNode()){
                nodeView->setContainedNodeViewItem((NodeViewItem*)item);
                MedeaDockWidget *dockWidget = MedeaWindowManager::constructViewDockWidget("SubView", Qt::TopDockWidgetArea);
                dockWidget->setWidget(nodeView);
                dockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
                dockWidget->setParent(this);
                innerWindow->addDockWidget(Qt::TopDockWidgetArea, dockWidget);
                //Get children.
            }
        }
    }
}

void MedeaMainWindow::popupSearch()
{
    if (searchBar->isVisible()) {
        searchBar->setFocus();
    } else {
        QPointF s = QPointF(searchToolbar->sizeHint().width(), searchToolbar->height());
        QPointF p = pos() + rect().center();
        p -= s;
        searchToolbar->move(p.x(), p.y());
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

void MedeaMainWindow::settingChanged(QString group, QString name, QVariant value)
{

}

void MedeaMainWindow::settingsApplied()
{

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
    QFont font = QFont(opensans_fontname);
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPointSizeF(8.5);
    QApplication::setFont(font);
}

void MedeaMainWindow::initializeSettings()
{
    qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
    SettingsController::settings();
    qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();
    qCritical() << "Settings loaded in: " <<  timeFinish-timeStart << "MS";
}

void MedeaMainWindow::initializeTheme()
{
    qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
    Theme::theme()->setIconToggledImage("Actions", "Grid_On", "Actions", "Grid_Off");
    Theme::theme()->setIconToggledImage("Actions", "Fullscreen", "Actions", "Failure");
    Theme::theme()->setIconToggledImage("Actions", "Minimap", "Actions", "Invisible");
    Theme::theme()->setIconToggledImage("Actions", "Arrow_Down", "Actions", "Arrow_Up");
    Theme::theme()->setIconToggledImage("Actions", "SearchOptions", "Actions", "Arrow_Down");
    Theme::theme()->setIconToggledImage("Actions", "DockMaximize", "Actions", "Minimize");
    Theme::theme()->setIconToggledImage("Actions", "Lock_Open", "Actions", "Lock_Closed");
    Theme::theme()->setIconToggledImage("Actions", "Invisible", "Actions", "Visible");

    qCritical() << "GUI THREAD: " << QThread::currentThread();
    //connect(this, &MedeaMainWindow::preloadImages, Theme::theme(), &Theme::initPreloadImages);
    //emit preloadImages();

    qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();
    qCritical() << "Theme initialized in: " <<  timeFinish - timeStart << "MS";
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

    MedeaDockWidget *dockWidget1 = MedeaWindowManager::constructNodeViewDockWidget("Interface", Qt::TopDockWidgetArea);
    dockWidget1->setWidget(nodeView_Interfaces);
    dockWidget1->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dockWidget1->setParent(this);

    MedeaDockWidget *dockWidget2 = MedeaWindowManager::constructNodeViewDockWidget("Behaviour", Qt::TopDockWidgetArea);
    dockWidget2->setWidget(nodeView_Behaviour);
    dockWidget2->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    MedeaDockWidget *dockWidget3 = MedeaWindowManager::constructNodeViewDockWidget("Assemblies", Qt::BottomDockWidgetArea);
    dockWidget3->setWidget(nodeView_Assemblies);
    dockWidget3->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    MedeaDockWidget *dockWidget4 = MedeaWindowManager::constructNodeViewDockWidget("Hardware", Qt::BottomDockWidgetArea);
    dockWidget4->setWidget(nodeView_Hardware);
    dockWidget4->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    dockWidget1->setProtected(true);
    dockWidget2->setProtected(true);
    dockWidget3->setProtected(true);
    dockWidget4->setProtected(true);

    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dockWidget1);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dockWidget2);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dockWidget3);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dockWidget4);

    connectNodeView(nodeView_Interfaces);
    connectNodeView(nodeView_Behaviour);
    connectNodeView(nodeView_Assemblies);
    connectNodeView(nodeView_Hardware);

    MedeaDockWidget *qosDockWidget = MedeaWindowManager::constructViewDockWidget("QOS Browser");
    qosBrowser = new QOSBrowser(viewController, this);
    qosDockWidget->setWidget(qosBrowser);
    qosDockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    qosDockWidget->setProtected(true);

    innerWindow->addDockWidget(Qt::TopDockWidgetArea, qosDockWidget);
    qosDockWidget->setVisible(false);
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
    setMenuBar(menuBar);

    menuBar->setNativeMenuBar(false);
}

void MedeaMainWindow::setupToolBar()
{
    floatingToolbar = new QToolBar(this);
    floatingToolbar->setMovable(false);
    floatingToolbar->setFloatable(false);
    floatingToolbar->setIconSize(QSize(20,20));
    //floatingToolbar->setIconSize(QSize(24,24));

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

    QToolBar* searchToolbar = new QToolBar(this);
    searchToolbar->setIconSize(QSize(18,18));
    searchToolbar->setFixedHeight(menuBar->height() - 6);
    searchToolbar->addWidget(searchBar);
    searchToolbar->addWidget(searchButton);
    searchToolbar->addWidget(searchOptionsButton);

    //searchToolbar->hide();
    menuBar->setCornerWidget(searchToolbar);
}

void MedeaMainWindow::setupPopupSearchBar()
{
    popupSearchButton = new QToolButton(this);
    popupSearchButton->setToolTip("Submit Search");

    popupSearchBar = new QLineEdit(this);
    popupSearchBar->setFont(QFont(font().family(), 14));
    popupSearchBar->setPlaceholderText("Search Here...");
    popupSearchBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    searchToolbar = new QToolBar(this);
    searchToolbar->setIconSize(QSize(24,24));
    searchToolbar->setFixedSize(300, 45);
    searchToolbar->addWidget(popupSearchBar);
    searchToolbar->addWidget(popupSearchButton);

    searchToolbar->setMovable(false);
    searchToolbar->setFloatable(false);
    searchToolbar->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    searchToolbar->setAttribute(Qt::WA_NoSystemBackground, true);
    searchToolbar->setAttribute(Qt::WA_TranslucentBackground, true);
    searchToolbar->setObjectName("POPUP_WIDGET");
    searchToolbar->hide();
}

void MedeaMainWindow::setupDataTable()
{
    tableWidget = new TableWidget(viewController, this);

    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("Table");
    dockWidget->setWidget(tableWidget);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget, Qt::Vertical);
}

void MedeaMainWindow::setupMinimap()
{
    minimap = new NodeViewMinimap(this);
    minimap->setFixedHeight(175);

    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("Minimap");
    dockWidget->setWidget(minimap);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget, Qt::Vertical);
}

void MedeaMainWindow::setupMainDockWidgetToggles()
{
    interfaceButton = new QToolButton(this);
    behaviourButton = new QToolButton(this);
    assemblyButton = new QToolButton(this);
    hardwareButton = new QToolButton(this);
    qosBrowserButton = new QToolButton(this);
    restoreDefaultButton = new QToolButton(this);

    QToolBar* toolbar = new QToolBar(this);
    toolbar->setIconSize(QSize(20,20));
    toolbar->setFixedHeight(menuBar->height() - 6);

    toolbar->addWidget(qosBrowserButton);
    toolbar->addSeparator();
    toolbar->addWidget(interfaceButton);
    toolbar->addWidget(behaviourButton);
    toolbar->addWidget(assemblyButton);
    toolbar->addWidget(hardwareButton);
    toolbar->addSeparator();
    toolbar->addWidget(restoreDefaultButton);

    //menuBar->setCornerWidget(toolbar);
    toolbar->hide();
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
