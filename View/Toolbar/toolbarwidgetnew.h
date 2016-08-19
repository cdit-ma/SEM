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
#include <QRadioButton>
#include <QVBoxLayout>

class ToolbarWidgetNew : public QWidget
{
    Q_OBJECT
public:
    explicit ToolbarWidgetNew(ViewController *vc, QWidget *parent = 0);

public slots:
    void themeChanged();

    void showToolbar(QPoint globalPos, QPointF itemPos = QPointF());
    void setVisible(bool visible);

    void execMenu();
    void populateDynamicMenu();
    void menuActionTrigged(QAction* action);

    void viewItem_Destructed(int ID, ViewItem* viewItem);

    void addChildNode(QAction* action);

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

    QList<QAction*> constructSubMenuActions(QString triggeredActionKind);

    ViewController* viewController;
    ActionController* actionController;
    ToolActionController* toolbarController;
    QSize iconSize;

    QHash<QAction*, QMenu*> popupMenuHash;
    QHash<QMenu*, QString> dynamicMenuKeyHash;
    QHash<QString, QMenu*> adoptableKindsSubMenus;

    QFrame* mainFrame;
    QFrame* shadowFrame;
    QToolBar* toolbar;

    ActionGroup* mainGroup;
    ActionGroup* connectGroup;

    QMenu* addMenu;
    QMenu* hardwareMenu;
    QMenu* replicateMenu;
    QMenu* hardwareViewOptionMenu;

    QAction* addChildAction;
    QAction* connectAction;
    QAction* hardwareAction;
    QAction* disconnectHardwareAction;
    QAction* definitionAction;
    QAction* implementationAction;
    QAction* instancesAction;
    QAction* connectionsAction;
    QAction* replicateCountAction;
    QAction* hardwareViewOptionAction;

    QSpinBox* replicateCount;
    QToolButton* applyReplicateCountButton;

    QRadioButton* allNodes;
    QRadioButton* connectedNodes;
    QRadioButton* unconnectedNodes;

    QPointF itemPos;
};

#endif // TOOLBARWIDGETNEW_H
