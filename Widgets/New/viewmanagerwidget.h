#ifndef VIEWMANAGERWIDGET_H
#define VIEWMANAGERWIDGET_H
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>

#include "medeawindownew.h"
#include "medeadockwidget.h"

#include "medeawindowmanager.h"

class ViewManagerWidget;
class WindowItem : public QWidget{
    Q_OBJECT
public:
    WindowItem(ViewManagerWidget* manager, MedeaWindowNew* window);
private slots:

    void dockWidgetAdded(MedeaDockWidget* widget);
private:

    void setupLayout();
    ViewManagerWidget* manager;
    MedeaWindowNew* window;
    QWidget* dockContainer;
    QVBoxLayout* dockContainerLayout;
    QToolBar* windowToolbar;
};

class DockWindowItem: public QToolBar{
    Q_OBJECT
public:
    DockWindowItem(ViewManagerWidget* manager, MedeaDockWidget* dockWidget);
private:
    void setupLayout();
    ViewManagerWidget* manager;
    MedeaDockWidget* dockWidget;
};

class ViewManagerWidget : public QWidget
{
    Q_OBJECT
public:
    ViewManagerWidget(MedeaWindowManager* manager);
    DockWindowItem* getDockWindowItem(int ID);
    WindowItem* getWindowItem(int ID);
private slots:
    void themeChanged();
    void windowConstructed(MedeaWindowNew* window);
    void dockWidgetConstructed(MedeaDockWidget* dockWidget);

private:
    void setupLayout();
    QScrollArea* scrollArea;
    QWidget* windowArea;
    QVBoxLayout* scrollLayout;

    QHash<int, WindowItem*> windows;
    QHash<int, DockWindowItem*> dockWidgets;
};





#endif // VIEWMANAGERWIDGET_H
