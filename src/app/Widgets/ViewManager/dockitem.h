#ifndef DOCKITEM_H
#define DOCKITEM_H

#include "../../Widgets/DockWidgets/basedockwidget.h"
#include "../../Widgets/DockWidgets/defaultdockwidget.h"
#include "../../Controllers/WindowManager/windowmanager.h"

#include <QToolBar>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>

class DockItem : public QToolBar
{
    Q_OBJECT

public:
    DockItem(ViewManagerWidget* manager, BaseDockWidget* dockWidget);

private:
    void updateIcon();
    void themeChanged();
    void titleChanged();
    void setupLayout();

    QLabel* label = nullptr;
    QAction* iconAction = nullptr;
    ViewManagerWidget* manager = nullptr;
    BaseDockWidget* dockWidget = nullptr;

    bool isNodeViewDockWidget = false;
};

#endif // DOCKITEM_H