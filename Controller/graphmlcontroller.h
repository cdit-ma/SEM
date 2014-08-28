#ifndef GRAPHMLCONTROLLER_H
#define GRAPHMLCONTROLLER_H

#include <QObject>
#include <QHash>
#include <QStandardItemModel>
#include <QItemSelection>

#include <QDebug>
#include "../Model/model.h"
#include "../GUI/nodeview.h"


struct GUIContainer{
    NodeItem* nodeItem;
    QStandardItem* modelItem;
    Node* node;
};

class GraphMLController: public QObject
{
    Q_OBJECT
public:
    GraphMLController(Model* model, NodeView *view);
    ~GraphMLController();


    QStandardItemModel* getTreeModel();
signals:
    //View Signals
    void view_addNodeItem(NodeItem* nodeItem);
    void view_addNodeEdge(NodeEdge* nodeEdge);
    void view_centerNodeItem(NodeItem* nodeItem);

    void view_LabelChanged(QString value);
    void view_EnableGUI(bool enabled);


    //Model Signals
    void model_ImportGraphML(QStringList inputGraphML);
    void model_ExportGraphML(QString filePath);
    void model_MakeChildNode(Node* node);

public slots:
    //GUI Operation Slots.
    void view_ImportGraphML(QStringList inputGraphML);
    void view_ExportGraphML(QString filePath);

    void view_UpdateLabel(QString value);

    void model_WriteToFile(QString filePath, QString data);

    //Model Slots
    void model_MakeNode(Node* node);
    void model_MakeEdge(Edge* edge);

    void model_MadeNodeNew(GraphMLContainer* item);
    void model_RemoveNode(GraphMLContainer* item);

    //NodeItem Slots
    void nodeItem_Selected(NodeItem* nodeItem);
    void nodeItem_SetCentered(NodeItem* nodeItem);
    void nodeItem_MakeChildNode(Node* node);

    //NodeEdge Slots
    void nodeEdge_Selected(NodeEdge* nodeEdge);

    //View Slots

    //TreeView Slots
    void view_SetCentered(QModelIndex index);
    void view_SetSelected(QModelIndex index);


    void view_ControlPressed(bool isDown);
    void view_ShiftTriggered(bool isDown);
    void view_DeleteTriggered(bool isDown);
    void view_SelectAll();

    void view_EmptyScenePressed();

private:
    bool KEY_CONTROL_DOWN;
    bool KEY_SHIFT_DOWN;

    void deselectNodeItems(NodeItem* selectedNodeItem = 0);
    void selectNodeItem(NodeItem* selectedNodeItem);

    void hideAllMatches(Node* node);
    void resetMatches();
    void deleteSelectedNodeItems();

    NodeItem* getNodeItemFromNode(Node* node);
    GUIContainer* getGUIContainer(Node* node);
    GUIContainer* getGUIContainer(NodeItem* nodeItem);
    GUIContainer* getGUIContainer(QModelIndex nodeIndex);

    void removeNodeItem(NodeItem* nodeItem);

    QVector<NodeItem*> selectedNodeItems;
    QVector<GUIContainer*> nodeContainers;
    QVector<NodeItem*> nodeItems;

    NodeItem* currentNodeAspect;

    QStandardItemModel* treeModel;

    Model* model;
    NodeView* view;
};

#endif // GRAPHMLCONTROLLER_H
