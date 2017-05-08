#ifndef TOOLBARCONTROLLER_H
#define TOOLBARCONTROLLER_H

#include <QToolBar>
#include <QObject>

#include "../ViewController/viewitem.h"

#include "../../Utils/actiongroup.h"
#include "nodeviewitemaction.h"


#include "../../ModelController/nodekinds.h"

class ViewController;
class SelectionController;
class ToolbarController : public QObject
{
    Q_OBJECT
public:
    enum DOCK_TYPE{PARTS, DEFINITIONS, FUNCTIONS, HARDWARE};

    ToolbarController(ViewController* viewController);

    QList<NodeViewItemAction*> getDefinitionNodeActions(NODE_KIND node_kind);
    QList<NodeViewItemAction*> getWorkerFunctions();

    NodeViewItemAction* getNodeAction(int ID);

    EDGE_KIND getNodesEdgeKind(NODE_KIND kind);
    QList<NodeViewItemAction*> getEdgeActionsOfKind(EDGE_KIND kind);
    QList<NodeViewItemAction*> getExistingEdgeActionsOfKind(EDGE_KIND kind);

    RootAction* getConnectEdgeActionOfKind(EDGE_KIND kind);
    RootAction* getDisconnectEdgeActionOfKind(EDGE_KIND kind);

    QList<QAction*> getAdoptableKindsActions(bool stealth);
    QAction* getAdoptableKindsAction(bool stealth);

    QList<QAction*> getConnectedNodesActions(bool stealth);
    QAction* getConnectedNodesAction(bool stealth);


    QAction* getToolAction(QString hashKey, bool stealth);

    QStringList getKindsRequiringSubActions();
    
    QString getInfoActionKeyForAdoptableKind(QString kind);

    void addChildNode(NODE_KIND kind, QPointF position);
    void addEdge(int dstID, EDGE_KIND edgeKind);
    void removeEdge(int dstID, EDGE_KIND edgeKind);
    void addConnectedChildNode(int dstID, NODE_KIND kind, QPointF position);
    void addWorkerProcess(int processID, QPointF position);

    bool requiresSubAction(NODE_KIND kind);
signals:
    void hardwareCreated(int ID);
    void hardwareDestructed(int ID);
    void workerProcessCreated(int ID);
    void workerProcessDestructed(int ID);
    void workerWorkloadCreated(int ID);
public slots:
    void actionHoverEnter(int ID);
    void actionHoverLeave(int ID);

private slots:
    void themeChanged();
    void viewItem_Constructed(ViewItem* viewItem);
    void viewItem_Destructed(int ID, ViewItem*);

    void selectionChanged(int selected);
    void actionFinished();

private:
    void setupToolActions();
    void setupNodeActions();
    void setupEdgeActions();
    NodeViewItemAction* getNodeViewItemAction(int ID);

    RootAction* createRootAction(QString hashKey, QString actionName, QString iconPath="", QString aliasPath="");

    QHash<QString, RootAction*> toolActions;
    QHash<NODE_KIND, RootAction*> nodeKindActions;
    QHash<EDGE_KIND, RootAction*> connectEdgeKindActions;
    QHash<EDGE_KIND, RootAction*> disconnectEdgeKindActions;
    QHash<NODE_KIND, EDGE_KIND> connectedNodeEdgeKinds;
    QList<int> hardwareIDs;
    QList<int> workerProcessIDs;

    QHash<int, NodeViewItemAction*> actions;
    QHash<QString, QString> infoActionKeyHash;

public:
    ActionGroup* actionGroup;
    ViewController* viewController;
    SelectionController* selectionController;

    QStringList interfaceKinds;
    QStringList kindsWithSubActions;

    ActionGroup* adoptableKindsGroup;

};

#endif // TOOLBARCONTROLLER_H

