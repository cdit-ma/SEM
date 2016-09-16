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
    QList<NodeViewItemAction*> getWorkerFunctions();

    NodeViewItemAction* getNodeAction(int ID);

    QList<QAction*> getNodeActionsOfKind(QString kind, bool stealth);

    QList<NodeViewItemAction*> getEdgeActionsOfKind(Edge::EDGE_KIND kind);
    QList<NodeViewItemAction*> getExistingEdgeActionsOfKind(Edge::EDGE_KIND kind);

    RootAction* getEdgeActionOfKind(Edge::EDGE_KIND kind);

    QList<QAction*> getAdoptableKindsActions(bool stealth);
    QAction* getAdoptableKindsAction(bool stealth);

    QList<QAction*> getConnectedNodesActions(bool stealth);
    QAction* getConnectedNodesAction(bool stealth);


    QAction* getToolAction(QString hashKey, bool stealth);

    QStringList getKindsRequiringSubActions();
    QList<NodeViewItemAction*> getRequiredSubActionsForKind(QString kind);
    
    QString getInfoActionKeyForAdoptableKind(QString kind);

    void addChildNode(QString kind, QPointF position);
    void addEdge(int dstID, Edge::EDGE_KIND edgeKind=Edge::EC_UNDEFINED);
    void addConnectedChildNode(int dstID, QString kind, QPointF position);
    void addWorkerProcess(int processID, QPointF position);

signals:
    void hardwareCreated(int ID);
    void hardwareDestructed(int ID);

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
    QHash<QString, RootAction*> nodeKindActions;
    QHash<Edge::EDGE_KIND, RootAction*> edgeKindActions;

    QHash<int, NodeViewItemAction*> actions;
    QList<int> hardwareIDs;
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

