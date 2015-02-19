#ifndef NODEVIEW_H//
#define NODEVIEW_H

#include "../Controller/newcontroller.h"
#include "nodeitem.h"

#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QRubberBand>


class ToolbarWidget;

class NodeView : public QGraphicsView
{
    Q_OBJECT
public:
    NodeView(bool subView = false, QWidget *parent = 0);
    void setController(NewController* controller);
    void disconnectController();
    bool isSubView();
    bool getControlPressed();

    ~NodeView();

    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

    //bool guiCreated(GraphML* item);

    void forceSortViewAspects();
    void resetModel();

    QList<NodeItem *> getVisibleNodeItems();
    Node* getSelectedNode();


signals:
    void triggerAction(QString action);
    void updateViewPort(QRectF);
    void updateZoom(qreal zoom);
    void deletePressed(bool isDown);
    void controlPressed(bool isDown);
    void escapePressed(bool isDown);
    void shiftPressed(bool isDown);

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
    void sortDeployment();
    void centerNode(QString nodeLabel);

    void getAdoptableNodesList(Node* node);
    void getLegalNodesList(Node* src);
    void getComponentDefinitions(Node* node);

    void updateDockAdoptableNodesList(Node* node);
    void updateMenuList(QString action, QStringList* nodeKinds, QList<Node*>* nodes);

    void constructNode(QString kind, QPointF);
    void constructEdge(Node* src, Node* dst);
    void constructComponentInstance(Node* assm, Node* defn, QPointF center);

    void turnOnViewAspect(QString aspect);
    void setGoToMenuActions(QString action, bool);


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
    void view_LockCenteredGraphML(GraphML* graphML);

    void view_SortNode(Node* node);
    void view_SetOpacity(GraphML* graphML, qreal opacity);

    void view_ConstructNode();

    void constructNewView(Node* centeredOn);
    

    void sortEntireModel();
    void sortNode(Node* node, Node* topMostNode = 0);

    void fitToScreen();
    void centreNode(Node* node);

    void clearSelection();
    void disableDockButtons();

    void updateDockButtons(Node* node);
    void view_updateDockContainer(QString dockContainer);

    void view_sortModel();
    void view_centerViewAspects();

    void setGoToToolbarButtons(QString action, Node *node);

    void goToDefinition(Node* node, bool show = true);
    void goToImplementation(Node* node, bool show = true);

    void view_deleteSelectedNode();
    void view_ConstructNode(QString nodeKind);
    void view_ConstructEdge(Node* src, Node* dst);
    void view_ConstructComponentInstance(Node *assm, Node *defn, int sender);

    void updateToolbarMenuList(QString action, Node* node);
    void updateToolbarAdoptableNodesList(QStringList nodeKinds);
    void updateToolbarLegalNodesList(QList<Node*>* nodeList);
    void updateToolbarDefinitionsList(QList<Node*>* nodeList);

    void componentInstanceConstructed(Node* node);

private:
    void connectGraphMLItemToController(GraphMLItem* GUIItem, GraphML* graphML);

    void storeGraphMLItemInHash(GraphMLItem* item);
    bool removeGraphMLItemFromHash(QString ID);

    void nodeConstructed_signalUpdates(NodeItem* nodeItem);
    void nodeSelected_signalUpdates(Node *node);
    void edgeConstructed_signalUpdates(Node* src);

    Node* hasDefinition(Node* node);
    Node* hasImplementation(Node* node);

    NodeItem* getNodeItemFromGraphMLItem(GraphMLItem* item);
    NodeEdge* getEdgeItemFromGraphMLItem(GraphMLItem* item);
    GraphMLItem *getGraphMLItemFromGraphML(GraphML* item);

    GraphMLItem* getGraphMLItemFromHash(QString ID);

    NewController* controller;

    QHash<QString, GraphMLItem*> guiItems;
    QPointF menuPosition;

    QStringList currentAspects;
    //QPoint origin;
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


    QList<NodeItem*> getNodeItemsList();
    void showAllViewAspects();

    QList<Node*>* files;
    ToolbarWidget* toolbar;

    bool SUB_VIEW;
    QList<NodeView*> subViews;

protected:
    bool viewportEvent(QEvent *);

};

#endif // NODEVIEW_H
