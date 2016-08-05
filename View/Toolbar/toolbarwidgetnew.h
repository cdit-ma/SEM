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

public slots:
    void addChildNode();
    void showToolbar(QPoint globalPos, QPointF itemPos = QPointF());
    void setVisible(bool visible);

    void themeChanged();

    void execMenu();
    void populateDeploymentMenu();
    void menuActionTrigged(QAction* action);

    void viewItem_Destructed(int ID, ViewItem* viewItem);

private:
    void setupToolbar();
    void setupActions();
    void setupMenus();
    void setupSplitMenus();
    void setupAddChildMenu();
    void setupReplicateCountMenu();
    void setupHardwareViewOptionMenu();
    void setupInstancesMenu();

    void setupConnections();
    void clearDynamicMenus();

    QMenu* constructTopMenu(QAction* parentAction, bool instantPopup = true);
    QAction* getInfoAction(QString hashKey);

    ViewController* viewController;
    ActionController* actionController;
    ToolActionController* toolbarController;
    QSize iconSize;

    QHash<QAction*, QMenu*> popupMenuHash;
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
    QAction* disconnectHardwareAction;
    QAction* definitionAction;
    QAction* implementationAction;
    QAction* instancesAction;
    QAction* connectionsAction;
    QAction* replicateCountAction;

    QSpinBox* replicateCount;
    //QAction* applyReplicateCountButton;
    //QToolBar* replicateToolbar;
    QToolButton* applyReplicateCountButton;
    QAction* applyReplicateCountAction;

    QPointF itemPos;
};

#endif // TOOLBARWIDGETNEW_H
