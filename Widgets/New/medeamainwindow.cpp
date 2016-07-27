#include "medeamainwindow.h"
#include "medeaviewdockwidget.h"
#include "medeatooldockwidget.h"
#include "../../View/theme.h"
#include "selectioncontroller.h"
#include <QDebug>
#include <QHeaderView>
#include <QPushButton>
#include <QMenuBar>
#define TOOLBAR_HEIGHT 32


MedeaMainWindow::MedeaMainWindow(ViewController *vc, QWidget* parent):MedeaWindowNew(parent, MedeaWindowNew::MAIN_WINDOW)
{
    floatingToolbar = 0;
    viewController = vc;

    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    setMinimumSize(1000,600);
    showNormal();

    setupInnerWindow();
    setupTools();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(MedeaWindowManager::manager(), SIGNAL(activeViewDockWidgetChanged(MedeaViewDockWidget*,MedeaViewDockWidget*)), this, SLOT(activeViewDockWidgetChanged(MedeaViewDockWidget*, MedeaViewDockWidget*)));
    setViewController(vc);
}

void MedeaMainWindow::setViewController(ViewController *vc)
{
    viewController = vc;
    SelectionController* controller = vc->getSelectionController();

    connect(controller, SIGNAL(itemActiveSelectionChanged(ViewItem*,bool)), tableWidget, SLOT(itemActiveSelectionChanged(ViewItem*, bool)));
    connect(vc->getActionController()->view_viewInNewWindow, SIGNAL(triggered(bool)), this, SLOT(spawnSubView()));
}

void MedeaMainWindow::themeChanged()
{
    Theme* theme = Theme::theme();
    QString BGColor = theme->getBackgroundColorHex();
    QString disabledBGColor = theme->getDisabledBackgroundColorHex();
    QString altBGColor = theme->getAltBackgroundColorHex();
    QString textColor = theme->getTextColorHex(Theme::CR_NORMAL);
    QString textSelectedColor = theme->getTextColorHex(Theme::CR_SELECTED);
    QString textDisabledColor = theme->getTextColorHex(Theme::CR_DISABLED);
    QString highlightColor = theme->getHighlightColorHex();
    QString pressedColor = theme->getPressedColorHex();

    setStyleSheet("QMainWindow {"
                  "background: " + BGColor + ";"
                  "color: " + textColor + ";"
                  "}"
                  "QGraphicsView {"
                  "background: " + BGColor + ";"
                  "border: 1px solid " + disabledBGColor + ";"
                  "}"
                  "QToolBar {"
                  "spacing: 3px;"
                  "border: none;"
                  "padding: 1px;"
                  "margin: 0px;"
                  "background:" + BGColor + ";"
                  "}"
                  "QToolBar::separator {"
                  "width: 5px;"
                  "background: rgba(0,0,0,0);"
                  "}"
                  "DockTitleBarWidget {"
                  "padding: 0px;"
                  "spacing: 0px;"
                  "background: " + altBGColor + ";"
                  "border: 1px solid " + disabledBGColor + ";"
                  "}"
                  "QToolButton {"
                  "padding: 2px;"
                  //"border: 1px solid " + disabledBGColor + ";"
                  "border-radius: 5px;"
                  "background:" + altBGColor + ";"
                  "}"
                  "QToolButton:hover {"
                  "background:" + highlightColor + ";"
                  "}"
                  "QToolButton:pressed {"
                  "background:" + pressedColor + ";"
                  "}"
                  "QPushButton {"
                  "background:" + altBGColor + ";"
                  "color:" + textColor + ";"
                  "border-radius: 5px;"
                  "border: 1px solid " + disabledBGColor + ";"
                  "}"
                  "QPushButton:hover {"
                  "background: " + highlightColor + ";"
                  "color:" + textSelectedColor + ";"
                  "}"
                  "QMenuBar {"
                  "padding: 3px;"
                  "background:" + BGColor + ";"
                  "border-bottom: 1px solid " + disabledBGColor + ";"
                  "}"
                  "QMenuBar::item {"
                  "background:" + BGColor + ";"
                  "color:" + textColor + ";"
                  "padding: 5px;"
                  "}"
                  "QMenuBar::item:selected {"
                  "color:" + textSelectedColor + ";"
                  "background:" + highlightColor + ";"
                  "border-radius: 2px;"
                  "}"
                  "QMenu {"
                  "background:" + altBGColor + ";"
                  //"border: 2px solid red;"
                  "border-radius: 5px;"
                  "margin: 2px 2px 2px 6px; "
                  "}"
                  "QMenu::item {"
                  "padding: 2px 15px 2px 25px;"
                  "background:" + altBGColor + ";"
                  "color:" + textColor + ";"
                  "border: none;"
                  "}"
                  "QMenu::item:disabled {"
                  "color:" + textDisabledColor + ";"
                  "}"
                  "QMenu::item:selected:!disabled {"
                  "color:" + textSelectedColor + ";"
                  "background: " + highlightColor + ";"
                  "border-radius: 2px;"
                  "}"
                  "QDockWidget {"
                  "margin: 5px;"
                  "background:" + BGColor + ";"
                  "}"
                  );

    viewController->getActionController()->menu_file->setStyleSheet(styleSheet());
    viewController->getActionController()->menu_file_recentProjects->setStyleSheet(styleSheet());
    viewController->getActionController()->menu_edit->setStyleSheet(styleSheet());
    viewController->getActionController()->menu_view->setStyleSheet(styleSheet());
    viewController->getActionController()->menu_model->setStyleSheet(styleSheet());
    viewController->getActionController()->menu_jenkins->setStyleSheet(styleSheet());
    viewController->getActionController()->menu_help->setStyleSheet(styleSheet());
    viewController->getActionController()->menu_window->setStyleSheet(styleSheet());
    viewController->getActionController()->menu_options->setStyleSheet(styleSheet());

    floatingToolbar->setStyleSheet("QToolButton{ padding: 4px; }");

    searchBar->setStyleSheet("QLineEdit {"
                             "background: " + altBGColor + ";"
                             "color:" + textColor + ";"
                             "border: 1px solid " + disabledBGColor + ";"
                             "}"
                             "QLineEdit:focus {"
                             "border-color:" + highlightColor + ";"
                             "}");

    //middlewareButton->setIcon(Theme::theme()->getIcon("Actions", "Arrow_Down"));
    //closeProjectButton->setIcon(Theme::theme()->getIcon("Actions", "Close"));
    searchButton->setIcon(Theme::theme()->getIcon("Actions", "Search"));
    searchOptionsButton->setIcon(Theme::theme()->getIcon("Actions", "Settings"));
}

void MedeaMainWindow::activeViewDockWidgetChanged(MedeaViewDockWidget *viewDock, MedeaViewDockWidget *prevDock)
{
    if(viewDock){
        NodeViewNew* view = viewDock->getNodeView();
        NodeViewNew* prevView = 0;
        if(prevDock){
            prevView = prevDock->getNodeView();
            if(prevView){
                disconnect(minimap, SIGNAL(minimap_Pan(QPointF)), prevView, SLOT(minimap_Pan(QPointF)));
                disconnect(minimap, SIGNAL(minimap_Panning(bool)), prevView, SLOT(minimap_Panning(bool)));
                disconnect(minimap, SIGNAL(minimap_Zoom(int)), prevView, SLOT(minimap_Zoom(int)));
                disconnect(prevView, SIGNAL(viewportChanged(QRectF, qreal)), minimap, SLOT(viewportRectChanged(QRectF, qreal)));
            }
        }

        if(view){
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
    /*
    QAction* action = (QAction*)sender();
    qCritical() << action->data();
    qCritical() << sender()->property("ID");
    qCritical() << sender()->property("ID2");
    qCritical() << sender()->property("ID3");
    */

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
    setupDataTable();
    setupMinimap();
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

    MedeaDockWidget *dockWidget1 = MedeaWindowManager::constructViewDockWidget("Interface", Qt::TopDockWidgetArea);
    dockWidget1->setWidget(nodeView_Interfaces);
    dockWidget1->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dockWidget1->setParent(this);

    MedeaDockWidget *dockWidget2 = MedeaWindowManager::constructViewDockWidget("Behaviour", Qt::TopDockWidgetArea);
    dockWidget2->setWidget(nodeView_Behaviour);
    dockWidget2->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    MedeaDockWidget *dockWidget3 = MedeaWindowManager::constructViewDockWidget("Assemblies", Qt::BottomDockWidgetArea);
    dockWidget3->setWidget(nodeView_Assemblies);
    dockWidget3->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    MedeaDockWidget *dockWidget4 = MedeaWindowManager::constructViewDockWidget("Hardware", Qt::BottomDockWidgetArea);
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
    menuBar = new QMenuBar(0);
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
    //addDockWidget(Qt::LeftDockWidgetArea, dockWidget, Qt::Vertical);
    //resizeEvent(0);
}

void MedeaMainWindow::setupSearchBar()
{   
    searchBar = new QLineEdit(this);
    searchBar->setFixedWidth(195);
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

    /*
    QWidget* holderWidget = new QWidget(this);
    QWidget* fillerWidget = new QWidget(this);
    fillerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(fillerWidget);
    layout->addWidget(searchToolbar);
    holderWidget->setLayout(layout);

    QToolBar* toolbar = new QToolBar(this);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setFixedHeight(TOOLBAR_HEIGHT);
    toolbar->addWidget(holderWidget);
    addToolBar(Qt::TopToolBarArea, toolbar);
    */

    menuBar->setCornerWidget(searchToolbar);
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

void MedeaMainWindow::resizeEvent(QResizeEvent *e)
{
    if(e){
        QMainWindow::resizeEvent(e);
    }
    if(floatingToolbar && floatingToolbar->orientation() == Qt::Vertical){
        floatingToolbar->setFixedHeight(centralWidget()->rect().height());
    }
}
