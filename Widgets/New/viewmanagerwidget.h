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
    ~WindowItem();
private slots:
    void themeChanged();
    void titleChanged(QString title="");

    void dockWidgetAdded(MedeaDockWidget* widget);
private:

    void setupLayout();
    QLabel* label;
    QAction* closeAction;
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
    void updateIcon();
    void themeChanged();
    void titleChanged();
    void setupLayout();
    QLabel* iconLabel;
    QLabel* label;

    QAction* iconAction;
    QAction* labelAction;
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
    void windowDestructed(int ID);
    void dockWidgetConstructed(MedeaDockWidget* dockWidget);
    void dockWidgetDestructed(int ID);

private:
    void setupLayout();
    QScrollArea* scrollArea;
    QWidget* windowArea;
    QVBoxLayout* scrollLayout;


    QHash<int, WindowItem*> windows;
    QHash<int, DockWindowItem*> dockWidgets;
};





#endif // VIEWMANAGERWIDGET_H
