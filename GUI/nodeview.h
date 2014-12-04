#ifndef NODEVIEW_H
#define NODEVIEW_H

#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include "nodeitem.h"
#include <QPointF>
#include <QRubberBand>

class NewController;

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

signals:
    void updateZoom(qreal zoom);
    void deletePressed(bool isDown);
    void controlPressed(bool isDown);
    void escapePressed(bool isDown);
    void shiftPressed(bool isDown);

    void constructNodeItem(QPointF);


    void copy();
    void cut();
    void paste();

    void undo();
    void redo();

    void unselect();
    void selectAll();

    void updateNodeType(QString name);
    void updateViewAspects(QStringList aspects);
    void sortModel();


    void view_SetSelectedAttributeModel(AttributeTableModel* model);


    void view_ConstructMenu(QPoint);

public slots:
    void view_ConstructGraphMLGUI(GraphML* item);
    void view_ShowMenu(QPoint position, QList<QAction *> actions);
    void printErrorText(GraphML* graphml, QString text);
    void updateNodeTypeName(QString name);
    void removeNodeItem(NodeItem* item);
    void centreItem(GraphMLItem* item);
    void clearView();
    void depthChanged(int depth);

    void setRubberBandMode(bool On);
    void setViewAspects(QStringList aspects);

    void showContextMenu(QPoint position);

    void view_ConstructNodeGUI(Node* node);
    void view_ConstructEdgeGUI(Edge* edge);

    void view_DestructGraphMLGUI(QString ID);
    //void view_DestructGraphMLGUI(GraphML* graphML);

    void view_SelectGraphML(GraphML* graphML, bool setSelected=true);
    void view_CenterGraphML(GraphML* graphML);


    void view_SortNode(Node* node);
    void view_SetOpacity(GraphML* graphML, qreal opacity);



private:
    void connectGraphMLItemToController(GraphMLItem* GUIItem, GraphML* graphML);

    NewController* controller;


    //NodeItem* getNodeItemFromGraphML(GraphML* item);
    //NodeEdge* getNodeEdgeFromGraphML(GraphML* item);
    //NodeEdge* getNodeEdgeFromEdge(Edge* edge);
    //NodeItem* getNodeItemFromNode(Node* node);
    //GraphMLItem* getGraphMLItemFromGraphML(GraphML* item);

    //Node* getNodeFromGraphML(GraphML* item);
    //Edge* getEdgeFromGraphML(GraphML* item);

    //QVector<NodeItem*> nodeItems;
    //QVector<NodeEdge*> nodeEdges;


    NodeItem* getNodeItemFromGraphMLItem(GraphMLItem* item);
    NodeEdge* getEdgeItemFromGraphMLItem(GraphMLItem* item);



    void storeGraphMLItemInHash(GraphMLItem* item);

    bool removeGraphMLItemFromHash(QString ID);

    GraphMLItem *getGraphMLItemFromGraphML(GraphML* item);
    GraphMLItem* getGraphMLItemFromHash(QString ID);
    QHash<QString, GraphMLItem*> guiItems;


    QStringList currentAspects;
    QPoint origin;
    bool rubberBanding;
    bool once;
    QRubberBand* rubberBand;
    QString NodeType;
    QRectF getVisibleRect();
    bool CONTROL_DOWN;
    bool SHIFT_DOWN;
    qreal totalScaleFactor;

};

#endif // NODEVIEW_H
