#include "medeamainwindow.h"
#include <QDebug>
#include "medeaviewdockwidget.h"
#include "medeatooldockwidget.h"

MedeaMainWindow::MedeaMainWindow(QWidget* parent):MedeaWindowNew(parent, true)
{
    setMinimumSize(1000,600);
    showNormal();

    setupInnerWindow();
    setupTools();

    connect(MedeaWindowManager::manager(), SIGNAL(activeDockWidgetChanged(MedeaDockWidget*)), this, SLOT(activeDockWidgetChanged(MedeaDockWidget*)));
}

void MedeaMainWindow::setViewController(ViewController *vc)
{
    viewController = vc;
    nodeView_Interfaces->setViewController(vc);
    nodeView_Behaviour->setViewController(vc);
    nodeView_Assemblies->setViewController(vc);
    nodeView_Hardware->setViewController(vc);

    connect(viewController, SIGNAL(viewItemConstructed(ViewItem*)), nodeView_Interfaces, SLOT(viewItem_Constructed(ViewItem*)));
    connect(viewController, SIGNAL(viewItemConstructed(ViewItem*)), nodeView_Behaviour, SLOT(viewItem_Constructed(ViewItem*)));
    connect(viewController, SIGNAL(viewItemConstructed(ViewItem*)), nodeView_Assemblies, SLOT(viewItem_Constructed(ViewItem*)));
    connect(viewController, SIGNAL(viewItemConstructed(ViewItem*)), nodeView_Hardware, SLOT(viewItem_Constructed(ViewItem*)));


    connect(viewController, SIGNAL(viewItemDestructing(int,ViewItem*)), nodeView_Interfaces, SLOT(viewItemDestructing(int,ViewItem*)));
    connect(viewController, SIGNAL(viewItemDestructing(int,ViewItem*)), nodeView_Behaviour, SLOT(viewItemDestructing(int,ViewItem*)));
    connect(viewController, SIGNAL(viewItemDestructing(int,ViewItem*)), nodeView_Assemblies, SLOT(viewItemDestructing(int,ViewItem*)));
    connect(viewController, SIGNAL(viewItemDestructing(int,ViewItem*)), nodeView_Hardware, SLOT(viewItemDestructing(int,ViewItem*)));
}

void MedeaMainWindow::activeDockWidgetChanged(MedeaDockWidget *widget)
{
    if(widget){
        QWidget* content = widget->widget();
        NodeViewNew* view = qobject_cast<NodeViewNew*>(content);
        if(view){
            minimap->setScene(view->scene());
            minimap->disconnect();

            connect(minimap, SIGNAL(minimap_Pan(QPointF)), view, SLOT(minimap_Pan(QPointF)));
            connect(minimap, SIGNAL(minimap_Panning(bool)), view, SLOT(minimap_Panning(bool)));
            connect(minimap, SIGNAL(minimap_Zoom(int)), view, SLOT(minimap_Zoom(int)));
            connect(view, SIGNAL(viewportChanged(QRectF, qreal)), minimap, SLOT(viewportRectChanged(QRectF, qreal)));

            view->viewportChanged();
        }
    }
}

void MedeaMainWindow::setupInnerWindow()
{
    innerWindow = MedeaWindowManager::constructSubWindow();
    setCentralWidget(innerWindow);


    nodeView_Interfaces = new NodeViewNew(VA_INTERFACES);
    nodeView_Behaviour = new NodeViewNew(VA_BEHAVIOUR);
    nodeView_Assemblies = new NodeViewNew(VA_ASSEMBLIES);
    nodeView_Hardware = new NodeViewNew(VA_HARDWARE);




    MedeaDockWidget *dockWidget1 = MedeaWindowManager::constructViewDockWidget("Interface", Qt::TopDockWidgetArea);
    dockWidget1->setWidget(nodeView_Interfaces);
    dockWidget1->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

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

void MedeaMainWindow::setupTools()
{
    minimap = new NodeViewMinimap(this);
    tableView = new QTableView(this);

    MedeaDockWidget *dockWidget1 = MedeaWindowManager::constructToolDockWidget("Minimap");
    dockWidget1->setWidget(minimap);
    dockWidget1->setAllowedAreas(Qt::RightDockWidgetArea);

    MedeaDockWidget *dockWidget2 = MedeaWindowManager::constructToolDockWidget("Table");
    dockWidget2->setWidget(tableView);
    dockWidget2->setAllowedAreas(Qt::RightDockWidgetArea);

    addDockWidget(Qt::RightDockWidgetArea, dockWidget2, Qt::Vertical);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget1, Qt::Vertical);
}

