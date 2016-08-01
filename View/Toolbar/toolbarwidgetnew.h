#ifndef TOOLBARWIDGETNEW_H
#define TOOLBARWIDGETNEW_H

#include "../../Controller/viewcontroller.h"
#include "../../Controller/toolbarcontroller.h"

#include "../theme.h"

#include <QWidget>
#include <QFrame>
#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>

class ToolbarWidgetNew : public QWidget
{
    Q_OBJECT
public:
    explicit ToolbarWidgetNew(ViewController *vc, QWidget *parent = 0);

signals:
    void addChildNode();

public slots:
    void themeChanged();
    void setVisible(bool visible);
    void viewItem_Destructed(int ID, ViewItem* viewItem);

    void execMenu();

private:
    void setupToolbar();
    void setupActions();
    void setupMenus();
    void setupAddChildMenu();

    ViewController* viewController;
    ActionController* actionController;
    ToolActionController* toolbarController;
    QSize iconSize;

    QHash<QAction*, QMenu*> actionMenuHash;
    QHash<QString, QMenu> adoptableKindsSubMenus;

    //QHash<int, NodeViewItemAction*> nodeActions;
    //QHash<int, QMenu> nodeActions;

    QFrame* mainFrame;
    QFrame* shadowFrame;
    QToolBar* toolbar;

    ActionGroup* mainGroup;
    ActionGroup* connectGroup;

    QMenu* addMenu;

    QAction* addChildAction;
    QAction* connectAction;
    QAction* instancesAction;
    QAction* connectionsAction;

};

#endif // TOOLBARWIDGETNEW_H
