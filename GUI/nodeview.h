#ifndef NODEVIEW_H//
#define NODEVIEW_H

#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include "nodeitem.h"
#include <QPointF>
#include <QRubberBand>
#include "../Controller/newcontroller.h"
//class ActionArray;
class NodeView : public QGraphicsView
{
    Q_OBJECT
public:
    NodeView(QWidget *parent = 0);
    void setController(NewController* controller);
    bool getControlPressed();
    ~NodeView();

    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

    //bool guiCreated(GraphML* item);

    void resetModel();

signals:
    void updateZoom(qreal zoom);
    void deletePressed(bool isDown);
    void controlPressed(bool isDown);
    void escapePressed(bool isDown);
    void shiftPressed(bool isDown);

    void constructNodeItem(QString kind, QPointF);

    void copy();
    void cut();
    void paste();

    void undo();
    void redo();

    void unselect();
    void selectAll();

    void updateNodeType(QString name);
    void updateViewAspects(QStringList aspects);

    void view_SetSelectedAttributeModel(AttributeTableModel* model);

    void view_ConstructMenu(QPoint);



    void componentNodeMade(QString type, NodeItem* nodeItem);
    void hardwareNodeMade(QString type, NodeItem* nodeItem);

    void updateDataTable();
    void updateViewMargin();
    void updateAdoptableNodeList(Node* node);
    void updateDockButtons(QString dockButton);
    void updateDockContainer(QString container);

    void centerModel();
    void sortModel();


public slots:
    void view_Refresh();
    void view_ConstructGraphMLGUI(GraphML* item);
    void printErrorText(GraphML* graphml, QString text);
    void updateNodeTypeName(QString name);
    void removeNodeItem(NodeItem* item);
    void centreItem(GraphMLItem* item);
    void clearView();
    void depthChanged(int depth);

    void setRubberBandMode(bool On);
    void setViewAspects(QStringList aspects);

    void showContextMenu(QPoint position);

    void view_DockConstructNode(QString kind);
    void view_ConstructNodeGUI(Node* node);
    void view_ConstructEdgeGUI(Edge* edge);

    void view_DestructGraphMLGUI(QString ID);
    //void view_DestructGraphMLGUI(GraphML* graphML);

    void view_SelectGraphML(GraphML* graphML, bool setSelected=true);
    void view_CenterGraphML(GraphML* graphML);

    void view_SortNode(Node* node);
    void view_SetOpacity(GraphML* graphML, qreal opacity);

    void view_ConstructNodeAction();



    void fitToScreen();
    void resetSceneRect(NodeItem *nodeItem);
    void centreModel(Node* node);

    void clearSelection();
    void updateDockButtons(Node* node);

    void view_addComponentDefinition(NodeItem* itm);
    void view_centerModel();

private:
    void connectGraphMLItemToController(GraphMLItem* GUIItem, GraphML* graphML);

    void storeGraphMLItemInHash(GraphMLItem* item);
    bool removeGraphMLItemFromHash(QString ID);

    NodeItem* getNodeItemFromGraphMLItem(GraphMLItem* item);
    NodeEdge* getEdgeItemFromGraphMLItem(GraphMLItem* item);
    GraphMLItem *getGraphMLItemFromGraphML(GraphML* item);
    GraphMLItem* getGraphMLItemFromHash(QString ID);


    NewController* controller;

    QHash<QString, GraphMLItem*> guiItems;
    QPointF menuPosition;

    QStringList currentAspects;
    QPoint origin;
    QRubberBand* rubberBand;
    QString NodeType;
    qreal totalScaleFactor;

    bool rubberBanding;
    bool once;
    QRectF getVisibleRect();
    bool CONTROL_DOWN;
    bool SHIFT_DOWN;


    double origRatio;
    bool firstSort;

    void sortInitialItems(QStringList aspects);
};

#endif // NODEVIEW_H
