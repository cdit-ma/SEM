#ifndef DOCKITEM_H
#define DOCKITEM_H

#include "../../Widgets/DockWidgets/basedockwidget.h"
#include "../../Controllers/WindowManager/windowmanager.h"

#include <QToolBar>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>


class DockItem: public QToolBar{
    Q_OBJECT
public:
    DockItem(ViewManagerWidget* manager, BaseDockWidget* dockWidget);
private:
    void updateIcon();
    void themeChanged();
    void titleChanged();
    void setupLayout();
    QLabel* iconLabel;
    QLabel* label;

    QAction* iconAction;
    QAction* labelAction;
    ViewManagerWidget* manager;
    BaseDockWidget* dockWidget;
};
#endif // DOCKITEM_H
