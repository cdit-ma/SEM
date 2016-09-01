#ifndef TOOLBARCONTROLLER_H
#define TOOLBARCONTROLLER_H

#include <QToolBar>
#include <QObject>
#include "../View/viewitem.h"
#include "nodeviewitemaction.h"
#include "../Widgets/New/actiongroup.h"
#include "rootaction.h"
#include "../Model/edge.h"

class ViewController;
class SelectionController;
class ToolActionController : public QObject
{
    Q_OBJECT
public:
    enum DOCK_TYPE{PARTS, DEFINITIONS, FUNCTIONS, HARDWARE};

    ToolActionController(ViewController* viewController);

    QList<NodeViewItemAction*> getDefinitionNodeActions(QString kind);

    QList<QAction*> getNodeActionsOfKind(QString kind, bool stealth);
    QAction* getNodeActionOfKind(QString kind, bool stealth);

    QList<NodeViewItemAction*> getEdgeActionsOfKind(Edge::EDGE_CLASS kind);

    RootAction* getEdgeActionOfKind(Edge::EDGE_CLASS kind);

    QList<QAction*> getAdoptableKindsActions(bool stealth);
    QAction* getAdoptableKindsAction(bool stealth);

    QList<QAction*> getConnectedNodesActions(bool stealth);
    QAction* getConnectedNodesAction(bool stealth);

    QList<QAction*> getHardwareActions(bool stealth);
    QAction* getHardwareAction(bool stealth);

    QList<QAction*> getInstancesActions(bool stealth);
    QAction* getInstancesAction(bool stealth);

    QAction* getToolAction(QString hashKey, bool stealth);

    QStringList getKindsRequiringSubActions();
    QList<NodeViewItemAction*> getRequiredSubActionsForKind(QString kind);

    void addChildNode(QString kind, QPointF position);
    void addEdge(int dstID, Edge::EDGE_CLASS edgeKind=Edge::EC_UNDEFINED);
    void addConnectedChildNode(int dstID, QString kind, QPointF position);

private slots:
    void themeChanged();
    void viewItem_Constructed(ViewItem* viewItem);
    void viewItem_Destructed(int ID, ViewItem* viewItem);

    void selectionChanged(int selected);
    void actionFinished();

private:
    void setupToolActions();
    void setupNodeActions();
    void setupEdgeActions();
    NodeViewItemAction* getNodeViewItemAction(int ID);

    RootAction* createRootAction(QString hashKey, QString actionName, QString iconPath="", QString aliasPath="");

    QHash<QString, RootAction*> toolActions;
    QHash<QString, RootAction*> nodeKindActions;
    QHash<Edge::EDGE_CLASS, RootAction*> edgeKindActions;

    QHash<int, NodeViewItemAction*> actions;

public:
    ActionGroup* actionGroup;
    ViewController* viewController;
    SelectionController* selectionController;

    QStringList interfaceKinds;
    QStringList kindsWithSubActions;

    ActionGroup* adoptableKindsGroup;

};

#endif // TOOLBARCONTROLLER_H
