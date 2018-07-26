#ifndef VIEWMANAGERWIDGET_H
#define VIEWMANAGERWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHash>

#include "../../Widgets/Windows/basewindow.h"
#include "../../Widgets/DockWidgets/basedockwidget.h"
#include "../../Controllers/WindowManager/windowmanager.h"

class WindowItem;
class DockItem;
class ViewManagerWidget : public QWidget
{
    Q_OBJECT
public:
    ViewManagerWidget(WindowManager* manager);
    DockItem* getDockItem(int ID);
private slots:
    void themeChanged();

    void windowConstructed(BaseWindow* window);
    void windowDestructed(int ID);

    void dockWidgetConstructed(BaseDockWidget* dockWidget);
    void dockWidgetDestructed(int ID);

private:
    void setupLayout();
    QScrollArea* scrollArea;
    QWidget* windowArea;
    QVBoxLayout* scrollLayout;


    QHash<int, WindowItem*> windowItems;
    QHash<int, DockItem*> dockItems;
};





#endif // VIEWMANAGERWIDGET_H
