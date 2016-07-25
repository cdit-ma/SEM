#include "medeamainwindow.h"
#include "medeaviewdockwidget.h"
#include "medeatooldockwidget.h"
#include "../../View/theme.h"
#include "selectioncontroller.h"
#include <QDebug>
#include <QHeaderView>
#include <QPushButton>


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
    QString altBGColor = theme->getAltBackgroundColorHex();
    QString textColor = theme->getTextColorHex(Theme::CR_NORMAL);
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
                  "margin:1px 0px;"
                  "background: " + altBGColor + ";"
                  "border:1px solid " + BGColor + ";"
                  "}"
                  "QToolButton {"
                  "padding:0px;"
                  "border: none;"
                  "border-radius: 2px;"
                  "background:" + altBGColor + ";"
                  "}"
                  "QToolButton:hover {"
                  "background:" + highlightColor + ";"
                  "}"
                  "QToolButton:pressed {"
                  "background:" + pressedColor + ";"
                  "}"
                  "QTableView::item {"
                  "padding: 0px 4px;"
                  "}"
                  );

    if (tableWidget && tableWidget->parentWidget()) {
        tableWidget->parentWidget()->setStyleSheet("QDockWidget{ background: " + BGColor + ";}");
    }
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

void MedeaMainWindow::setupDataTable()
{
    tableWidget = new TableWidget(this);
    qCritical() << "YELLO!";

    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("Table");
    dockWidget->setWidget(tableWidget);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget, Qt::Vertical);
    connect(dockWidget, SIGNAL(topLevelChanged(bool)), this, SLOT(updateDataTableSize()));
}

void MedeaMainWindow::setupMinimap()
{
    minimap = new NodeViewMinimap(this);
    minimap->setFixedHeight(200);

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

void MedeaMainWindow::updateDataTableSize()
{

}























