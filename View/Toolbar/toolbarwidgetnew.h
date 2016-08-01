#ifndef TOOLBARWIDGETNEW_H
#define TOOLBARWIDGETNEW_H

#include "../../Controller/viewcontroller.h"
#include "../../Controller/toolbarcontroller.h"

#include "../theme.h"

#include <QWidget>
#include <QFrame>
#include <QToolBar>
#include <QAction>
#include <QWidgetAction>
#include <QToolButton>
#include <QSpinBox>
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
    void setupSplitMenus();
    void setupAddChildMenu();
    void setupHardwareMenu();
    void setupReplicateCountMenu();
    void setupHardwareViewOptionMenu();
    void setupInstancesMenu();

    ViewController* viewController;
    ActionController* actionController;
    ToolActionController* toolbarController;
    QSize iconSize;

    QHash<QAction*, QMenu*> actionMenuHash;
    QHash<QString, QMenu*> adoptableKindsSubMenus;

    //QHash<int, NodeViewItemAction*> nodeActions;
    //QHash<int, QMenu> nodeActions;

    QFrame* mainFrame;
    QFrame* shadowFrame;
    QToolBar* toolbar;

    ActionGroup* mainGroup;
    ActionGroup* connectGroup;

    QMenu* addMenu;
    QMenu* hardwareMenu;
    QMenu* replicateMenu;

    QAction* addChildAction;
    QAction* connectAction;
    QAction* hardwareAction;
    QAction* definitionAction;
    QAction* implementationAction;
    QAction* instancesAction;
    QAction* connectionsAction;
    QAction* replicateCountAction;

    QSpinBox* replicateCount;
    QToolButton* applyReplicateCountButton;

};

#endif // TOOLBARWIDGETNEW_H
