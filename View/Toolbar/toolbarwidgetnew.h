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
private:
    void setupToolbar();
    void setupAddChildMenus();

    ViewController* viewController;
    ActionController* actionController;
    ToolActionController* toolbarController;
    QSize iconSize;


    QHash<QString, QMenu> adoptableKindsSubMenus;

    //QHash<int, NodeViewItemAction*> nodeActions;
    //QHash<int, QMenu> nodeActions;


    QFrame* mainFrame;
    QFrame* shadowFrame;
    QToolBar* toolbar;


    QAction* addChildAction;
};

#endif // TOOLBARWIDGETNEW_H
