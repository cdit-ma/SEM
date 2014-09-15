#ifndef GRAPHMLCONTROLLER_H
#define GRAPHMLCONTROLLER_H

#include <QObject>
#include <QHash>
#include <QStandardItemModel>
#include <QItemSelection>

#include <QDebug>
#include "../Model/model.h"
#include "../GUI/nodeview.h"
#include <QClipboard>
#include <QThread>
#include <QInputDialog>
#include <QHash>
#include <QStack>
enum ACTION_TYPE {CONSTRUCT, DESTRUCT, MODIFIED};

struct GUIContainer{
    NodeItem* nodeItem;
    QStandardItem* modelItem;
    QString deleteXML;
    QString parentNodeID;
    Node* node;
};

struct Action{
    GraphML::KIND itemKind;
    ACTION_TYPE actionType;
    QString itemID;
    QString srcID;
    QString dstID;
    QString parentID;
    QString removedXML;
    QString key;
    QString previousValue;
    QString actionName;
    int actionID;
};

class GraphMLController: public QObject
{
    Q_OBJECT
public:

    GraphMLController(NodeView *view);
    ~GraphMLController();


    Model* getModel();

    QStandardItemModel* getTreeModel();
signals:
    //View Signals
    void view_addNodeItem(NodeItem* nodeItem);
    void view_addNodeEdge(NodeEdge* nodeEdge);
    void view_centerNodeItem(NodeItem* nodeItem);

    void view_LabelChanged(QString value);
    void view_EnableGUI(bool enabled);

    void view_CopyText(QString value);

    //Model Signals
    void model_ImportGraphML(QStringList inputGraphML, GraphMLContainer *currentParent=0);
    void model_ImportGraphML(QString, GraphMLContainer *currentParent);
    void model_ExportGraphML(QString filePath);

    void model_ConstructNode(QString kind, GraphMLContainer* parentNode);
    void model_ConstructEdge(GraphMLContainer* src, GraphMLContainer* dst);


public slots:

    //MODEL SLOTS
    //Functions triggered by the Model
    void model_MadeEdge(Edge* edge);
    void model_RemoveEdge(Edge* edge, QString srcID, QString dstID);

    void model_MadeNode(GraphMLContainer* item);
    void model_RemoveNode(GraphMLContainer* item, QString ID);

    void model_WriteToFile(QString filePath, QString data);
    void model_EnableGUI(bool enabled);

    //VIEW SLOTS
    //Functions triggered by the View.
    void view_ControlPressed(bool isDown);
    void view_ShiftTriggered(bool isDown);
    void view_DeleteTriggered(bool isDown);
    void view_SelectAll();
    void view_EmptyScenePressed();

    void view_ActionTriggered(QString action);

    //Functions triggered by GUI Operation Slots.
    void view_ImportGraphML(QStringList inputGraphML);
    void view_ExportGraphML(QString filePath);
    void view_UpdateLabel(QString value);

    //Copy/Paste Operations
    void view_Undo();
    void view_Redo();
    void view_Copy();
    void view_Cut();
    void view_Paste(QString XMLData);

    //NODEITEM SLOTS
    //Functions triggered by the NodeItems in the View
    void nodeItem_Selected(NodeItem* nodeItem);
    void nodeItem_SetCentered(NodeItem* nodeItem);
    void nodeItem_MakeChildNode(QString type, Node* node);


    void view_updateGraphMLData(Node* node, QString key, QString value);

    //NODEEDGE SLOTS
    //Functions triggered by the NodeEdges in the View
    void nodeEdge_Selected(NodeEdge* nodeEdge);


    //TREEVIEW SLOTs
    //Functions Triggered by the TreeModel in the View
    void view_SetCentered(QModelIndex index);
    void view_SetSelected(QModelIndex index);

private:
    void undoRedo(bool UNDO);
    bool KEY_CONTROL_DOWN;
    bool KEY_SHIFT_DOWN;

    void deselectNodeItems(NodeItem* selectedNodeItem = 0);
    void deselectEdgeItems(NodeEdge* selectedEdgeItem = 0);
    void selectNodeItem(NodeItem* selectedNodeItem);
    void selectEdgeItem(NodeEdge* selectedEdgeItem);

    GraphMLContainer* getSingleSelectedNode();
    NodeItem* getSingleSelectedNodeItem();

    void hideAllMatches(Node* node);
    void resetMatches();
    void deleteSelectedNodeItems();
    void deleteSelectedEdgeItems();

    NodeItem* getNodeItemFromNode(Node* node);
    NodeEdge* getNodeEdgeFromEdge(Edge* edge);
    GUIContainer* getGUIContainer(Node* node);
    GUIContainer* getGUIContainer(NodeItem* nodeItem);
    GUIContainer* getGUIContainer(QModelIndex nodeIndex);


    void addActionToStack(Action action);
    void reverseAction(Action action);
    void removeGraphML(GraphML* node);
    void removeNodeItem(NodeItem* nodeItem);
    void removeNodeEdge(NodeEdge* nodeEdge);

    QVector<NodeItem*> selectedNodeItems;

    QVector<NodeEdge*> selectedEdgeItems;

    QStack<Action> reversingStack;
    QStack<Action> undoStack;
    QStack<Action> redoStack;

    QStack<QString> undoIDStack;

    QVector<GUIContainer*> nodeContainers;
    QVector<NodeItem*> nodeItems;
    QVector<NodeEdge*> edgeItems;

    QHash<QString, GraphML*> undoLookup;
    QHash<QString, GraphMLContainer*> nodeDeletionIDLookup;

    NodeItem* currentMaximized;

    Node* currentParent;

    QStandardItemModel* treeModel;

    QString copyBuffer;

    QString currentAction;
    int currentActionID;

    int actionCount;
    bool actionWorking;

    QThread* modelThread;

    Model* model;
    NodeView* view;
    bool UNDOING;
    bool REDOING;
};

#endif // GRAPHMLCONTROLLER_H
