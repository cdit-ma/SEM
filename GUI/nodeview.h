#ifndef NODEVIEW_H//
#define NODEVIEW_H

#include "../Controller/newcontroller.h"
#include "nodeitem.h"
#include "toolbarwidget.h"

#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QRubberBand>

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

    QList<NodeItem *> getVisibleNodeItems();

signals:
    void updateViewPort(QRectF);
    void updateZoom(qreal zoom);
    void deletePressed(bool isDown);
    void controlPressed(bool isDown);
    void escapePressed(bool isDown);
    void shiftPressed(bool isDown);

    void constructNodeItem(QString kind, QPointF);
    void constructEdgeItem(Node* src, Node* dst);

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
    void updateDockButtons(QString dockButton);
    void updateDockContainer(QString container);

    void sortModel();
    void centerNode(QString nodeLabel);

    void hideDeployment(bool hide);
    void hideToolbarWidget();

    void hasSelectedNode(bool nodeSelected);

    void getLegalNodesList(Node* src);
    void getAdoptableNodeList(Node* node);
    void updateDockAdoptableNodeList(Node* node);
    void updateMenuList(QString action, QStringList* nodeKinds, QList<Node*>* nodes);

public slots:

    void selectedInRubberBand(QPointF fromScenePoint, QPointF toScenePoint);
    void view_ConstructGraphMLGUI(GraphML* item);
    void printErrorText(GraphML* graphml, QString text);
    void removeNodeItem(NodeItem* item);
    void centreItem(GraphMLItem* item);
    void clearView();

    void setRubberBandMode(bool On);
    void setViewAspects(QStringList aspects);

    void showContextMenu(QPoint position);

    void view_DockConstructNode(Node* node, QString kind);
    void view_ConstructNodeGUI(Node* node);
    void view_ConstructEdgeGUI(Edge* edge);

    void view_DestructGraphMLGUI(QString ID);

    void view_SelectGraphML(GraphML* graphML, bool setSelected=true);
    void view_CenterGraphML(GraphML* graphML);

    void view_SortNode(Node* node);
    void view_SetOpacity(GraphML* graphML, qreal opacity);

    void view_ConstructNodeAction();


    void view_ConstructNodeAction(QString nodeKind);
    void view_ConstructEdgeAction(Node* src, Node* dst);
    
    void clearSelection();

    void fitToScreen();
    void centreNode(Node* node);

    void updateDockButtons(Node* node);
    void view_updateDockContainer(QString dockContainer);

    void view_sortModel();
    void view_centerViewAspects();

    void view_addComponentDefinition(NodeItem* itm);

    void goToDefinition(Node* node);
    void goToImplementation(Node* node);

    void trigger_shiftPressed();
    void trigger_deletePressed();

    void updateToolbarMenuList(QString action, Node* node);
    void updateToolbarAdoptableNodeList(QStringList nodeKinds);
    void updateToolbarLegalNodesList(QList<Node*>* nodeList);

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

    bool RUBBERBAND_MODE;
    bool drawingRubberBand;
    QPoint rubberBandOrigin;
    bool once;
    QRectF getVisibleRect();
    bool CONTROL_DOWN;
    bool SHIFT_DOWN;


    double origRatio;
    bool firstSort;

    QList<NodeItem*> getNodeItemsList();

    ToolbarWidget* toolbar;
    bool shiftTriggered;
    bool deleteTriggered;


protected:
    bool viewportEvent(QEvent *);

};

#endif // NODEVIEW_H
