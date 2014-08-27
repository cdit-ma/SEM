#ifndef GRAPHMLCONTROLLER_H
#define GRAPHMLCONTROLLER_H

#include <QObject>
#include <QHash>

#include <QDebug>
#include "../Model/model.h"
#include "../GUI/nodeview.h"




class GraphMLController: public QObject
{
    Q_OBJECT
public:
    GraphMLController(Model* model, NodeView *view);
    ~GraphMLController();


signals:
    //View Signals
    void view_addNodeItem(NodeItem* nodeItem);
    void view_addNodeEdge(NodeEdge* nodeEdge);
    void view_centerNodeItem(NodeItem* nodeItem);


    //Model Signals
    void model_ImportGraphML(QStringList inputGraphML);
    void model_ExportGraphML(QString filePath);



public slots:
    //GUI Operation Slots.
    void view_ImportGraphML(QStringList inputGraphML);
    void view_ExportGraphML(QString filePath);

    void model_WriteToFile(QString filePath, QString data);

    //Model Slots
    void model_MakeNode(Node* node);
    void model_MakeEdge(Edge* edge);

    void view_RemoveNodeItem(NodeItem* node);


    //NodeItem Slots
    void nodeItem_Selected(NodeItem* nodeItem);
    void nodeItem_SetCentered(NodeItem* nodeItem);

    //NodeEdge Slots
    void nodeEdge_Selected(NodeEdge* nodeEdge);

    //View Slots
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

    void deleteSelectedNodeItems();

    QVector<NodeItem*> selectedNodeItems;
    QVector<NodeItem*> allNodeItems;

    QHash<Node*,NodeItem*> nodeItemLookup;

    Model* model;
    NodeView* view;
};

#endif // GRAPHMLCONTROLLER_H
