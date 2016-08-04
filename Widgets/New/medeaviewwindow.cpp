#include "medeaviewwindow.h"
#include "medeaviewdockwidget.h"
#include "../../View/theme.h"
#include <QStringBuilder>
#include <QMenu>
MedeaViewWindow::MedeaViewWindow():MedeaWindowNew(0, MedeaWindowNew::VIEW_WINDOW)
{

    setAcceptDrops(true);
    setDockNestingEnabled(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    //Setup Tab positions
    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::West);

    //Setup Reset action
    resetDockedWidgetsAction = new QAction("Reset Docked Widgets", this);

    connect(resetDockedWidgetsAction, SIGNAL(triggered(bool)), this, SLOT(resetDockWidgets()));




    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}
void MedeaViewWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget *widget, Qt::Orientation orientation)
{
    MedeaViewDockWidget* dockWidget = qobject_cast<MedeaViewDockWidget*>(widget);
    if(dockWidget){
        //Only allow View Dock Widgets.
        MedeaWindowNew::addDockWidget(area, widget, orientation);
    }
}

void MedeaViewWindow::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet(theme->getWindowStyleSheet() %
                  theme->getViewStyleSheet() %
                  theme->getMenuBarStyleSheet() %
                  theme->getMenuStyleSheet() %
                  theme->getToolBarStyleSheet() %
                  theme->getDockWidgetStyleSheet() %
                  theme->getPushButtonStyleSheet() %
                  theme->getPopupWidgetStyleSheet() %
                  "QToolButton{ padding: 4px; }"
                  );

    resetDockedWidgetsAction->setIcon(Theme::theme()->getImage("Actions", "Maximize"));
}

void MedeaViewWindow::resetDockWidgets()
{
    foreach(MedeaDockWidget* child, getDockWidgets()){
        child->setVisible(true);
    }
}

QMenu *MedeaViewWindow::createPopupMenu()
{
    QMenu* menu = MedeaWindowNew::createPopupMenu();
    menu->addSeparator();
    menu->addAction(resetDockedWidgetsAction);
    return menu;
}


