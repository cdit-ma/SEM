#include "medeamainwindow.h"
#include "medeaviewdockwidget.h"
#include "medeatooldockwidget.h"
#include "../../View/theme.h"
#include "selectioncontroller.h"
#include <QDebug>
#include <QHeaderView>
#include <QPushButton>

#define TOOLBAR_HEIGHT 34


MedeaMainWindow::MedeaMainWindow(QWidget* parent):MedeaWindowNew(parent, MedeaWindowNew::MAIN_WINDOW)
{
    setMinimumSize(1000,600);
    showNormal();

    setupInnerWindow();
    setupTools();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(MedeaWindowManager::manager(), SIGNAL(activeViewDockWidgetChanged(MedeaViewDockWidget*,MedeaViewDockWidget*)), this, SLOT(activeViewDockWidgetChanged(MedeaViewDockWidget*, MedeaViewDockWidget*)));
}

void MedeaMainWindow::setViewController(ViewController *vc)
{
    viewController = vc;
    SelectionController* controller = vc->getSelectionController();

    connect(tableWidget, SIGNAL(cycleActiveItem(bool)), controller, SLOT(cycleActiveSelectedItem(bool)));
    connect(controller, SIGNAL(activeSelectedItemChanged(ViewItem*,bool)), tableWidget, SLOT(activeSelectedItemChanged(ViewItem*,bool)));
    connectNodeView(nodeView_Interfaces);
    connectNodeView(nodeView_Behaviour);
    connectNodeView(nodeView_Assemblies);
    connectNodeView(nodeView_Hardware);
}

void MedeaMainWindow::themeChanged()
{
    Theme* theme = Theme::theme();
    QString BGColor = theme->getBackgroundColorHex();
    QString disabledBGColor = theme->getDisabledBackgroundColorHex();
    QString altBGColor = theme->getAltBackgroundColorHex();
    QString textColor = theme->getTextColorHex(Theme::CR_NORMAL);
    QString textHighlightColor = theme->getTextColorHex(Theme::CR_SELECTED);
    QString textDisabledColor = theme->getTextColorHex(Theme::CR_DISABLED);
    QString highlightColor = theme->getHighlightColorHex();
    QString pressedColor = theme->getPressedColorHex();

    setStyleSheet("QMainWindow {"
                  "background: " + BGColor + ";"
                  "color: " + textColor + ";"
                  "}"
                  "QGraphicsView {"
                  "background: " + BGColor + ";"
                  "}"
                  "QToolBar {"
                  "spacing: 3px;"
                  "border: none;"
                  "padding: 1px;"
                  //"padding: 50px 0px 20px 30px;"
                  "margin: 0px;"
                  "background:" + BGColor + ";"
                  "}"
                  "DockTitleBarWidget {"
                  "padding: 1px;"
                  "background: " + altBGColor + ";"
                  "border:1px solid " + disabledBGColor + ";"
                  "}"
                  "QToolButton {"
                  "padding:0px;"
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
                  "QDockWidget{ background: " + BGColor + ";}"
                  "QPushButton{ background:" + altBGColor + "; color:" + textColor + "; border-radius: 5px; border: 1px solid " + disabledBGColor + "; }"
                  "QPushButton:hover{ background: " + highlightColor + "; color:" + textHighlightColor + "; }"
                  );

    searchBar->setStyleSheet("QLineEdit {"
                             "background: " + altBGColor + ";"
                             "color: " + textDisabledColor + ";"
                             "border: 1px solid " + disabledBGColor + ";"
                             "}"
                             "QLineEdit:focus {"
                             "border-color:" + highlightColor + ";"
                             "background: " + altBGColor + ";"
                             "color:" + textColor + ";"
                             "}");

    middlewareButton->setIcon(Theme::theme()->getIcon("Actions", "Arrow_Down"));
    closeProjectButton->setIcon(Theme::theme()->getIcon("Actions", "Close"));
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
    setupMenuAndTitle();
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
}

void MedeaMainWindow::setupMenuAndTitle()
{
    menuButton = new QPushButton(Theme::theme()->getIcon("Actions", "MEDEAIcon"), "", this);
    menuButton->setFixedSize(30, 30);
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
    //closeProjectButton->hide();

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
}

void MedeaMainWindow::setupToolBar()
{
    floatingToolbar = new QToolBar(this);

    QToolButton* b1 = new QToolButton(this);
    QToolButton* b2 = new QToolButton(this);
    QToolButton* b3 = new QToolButton(this);
    QToolButton* b4 = new QToolButton(this);
    QToolButton* b5 = new QToolButton(this);

    QWidget* w1 = new QWidget(this);
    QWidget* w2 = new QWidget(this);
    w1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    w2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    floatingToolbar->addWidget(w1);
    floatingToolbar->addWidget(b1);
    floatingToolbar->addWidget(b2);
    floatingToolbar->addWidget(b3);
    floatingToolbar->addWidget(b4);
    floatingToolbar->addWidget(b5);
    floatingToolbar->addWidget(w2);

    /*
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(w1);
    layout->addWidget(floatingToolbar);
    layout->addWidget(w2);

    QWidget* holderWidget = new QWidget(this);
    holderWidget->setLayout(layout);

    QToolBar* toolbar = new QToolBar(this);
    toolbar->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    toolbar->addWidget(holderWidget);
    addToolBar(Qt::TopToolBarArea, toolbar);
    */

    //addToolBarBreak();
    //addToolBar(Qt::TopToolBarArea, floatingToolbar);
    addToolBar(Qt::LeftToolBarArea, floatingToolbar);
}

void MedeaMainWindow::setupSearchBar()
{   
    searchBar = new QLineEdit(this);
    searchBar->setMinimumHeight(28);
    searchBar->setPlaceholderText("Search Here...");
    searchBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    searchButton = new QToolButton(this);
    searchButton->setToolTip("Submit Search");

    searchOptionsButton = new QToolButton(this);
    searchOptionsButton->setToolTip("Search Settings");

    QToolBar* searchToolbar = new QToolBar(this);
    searchToolbar->setFixedWidth(255);

    searchToolbar->addWidget(searchBar);
    searchToolbar->addWidget(searchButton);
    searchToolbar->addWidget(searchOptionsButton);

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
}

void MedeaMainWindow::setupDataTable()
{
    tableWidget = new TableWidget(this);

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

    MedeaDockWidget* dockWidget2 = MedeaWindowManager::constructToolDockWidget("Sub Views Yo");
    QPushButton* button = new QPushButton("Spawn SubView");
    button->setFixedHeight(50);
    dockWidget2->setWidget(button);
    dockWidget2->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget2, Qt::Vertical);

    connect(button, SIGNAL(clicked(bool)), this, SLOT(spawnSubView()));
}

