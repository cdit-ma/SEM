#ifndef NODEITEM_H
#define NODEITEM_H


#include "graphmlitem.h"
#include "../../Model/node.h"
#include "../../Controller/nodeadapter.h"
#include "../../enumerations.h"
class EdgeItem;
class NodeView;


class NodeItem : public GraphMLItem
{
    Q_OBJECT
public:
    enum RESIZE_TYPE{NO_RESIZE, RESIZE, HORIZONTAL_RESIZE, VERTICAL_RESIZE};

    NodeItem(NodeAdapter *node, GraphMLItem *parent, GUI_KIND kind);
    ~NodeItem();

    QRectF childrenBoundingRect();
    QRectF getChildBoundingRect();


    QString getLabel();
    VIEW_ASPECT getViewAspect();
    void setViewAspect(VIEW_ASPECT aspect);

    virtual QRectF gridRect() const = 0;
    virtual QRectF minimumRect() const;

    QPointF getCenterOffset() const;

    QPointF getMinimumRectCenter() const;
    QPointF getMinimumRectCenterPos() const;
    void setMinimumRectCenterPos(QPointF pos);

    void sortChildren();
    virtual void setPos(const QPointF pos) = 0;
    virtual qreal getItemMargin() const = 0;

    void updatePositionInModel(bool directUpdate = false);
    void updateSizeInModel(bool directUpdate = false);

    RESIZE_TYPE resizeEntered(QPointF mousePosition);

    void snapToGrid();

    void addEdge(EdgeItem* edge);
    void removeEdge(int ID);
    bool hasEdges();
    void addEdgeAsChild(EdgeItem* edge);
    void removeChildEdge(int ID);
    QList<EdgeItem*> getEdges();



    void resizeToOptimumSize(RESIZE_TYPE rt = RESIZE);
    void adjustSize(QSizeF delta);
    virtual void setWidth(qreal w) = 0;
    virtual void setHeight(qreal h) = 0;
    virtual qreal getWidth() const = 0;
    virtual qreal getHeight() const = 0;

    void showChildGridOutline(NodeItem* item, QPointF gridPoint);
    void hideChildGridOutline(int ID);
    QList<QRectF> getChildrenGridOutlines();
    void clearChildrenGridOutlines();
    void adjustPos(QPointF delta);




    QPointF getNextChildPos(QRectF childRect = QRectF(), QPainterPath childrenPath=QPainterPath(), bool useChildrenPath = false);
    QPointF getClosestGridPoint(QPointF childCenterPos);
    QPointF getGridPosition(int x, int y);

    QVector<QLineF> getGridLines();

    qreal getGridSize();
    qreal getGridMargin();

    bool drawGridLines();
    void setDrawGrid(bool drawGrid);
    bool isGridModeOn();

    NodeItem* getParentNodeItem();

    Node* getNode();

    RESIZE_TYPE getResizeMode();
    void setResizeMode(RESIZE_TYPE mode);

signals:
    void lastEdgeRemoved();
    void move(QPointF delta);
    void resize(QSizeF delta);
    void moveFinished();
    void resizeFinished();

    void NodeItem_ResizeSelection(int ID, QSizeF delta);
    void NodeItem_ResizeFinished(int ID);

public slots:
    void toggleGridMode(bool enabled);
    void dataChanged(QString keyName, QVariant data) = 0;
    virtual void childPositionChanged() = 0;
    virtual void childSizeChanged() = 0;

    void updateGrid();


private:
    bool IS_GRID_MODE_ON;
    bool DRAW_GRID;

    VIEW_ASPECT viewAspect;


    QVector<QLineF> verticalGridLines;
    QVector<QLineF> horizontalGridLines;

    QHash<int, QRectF> childrenGridOutlines;
    QHash<int, EdgeItem*> connectedEdges;
    QHash<int, EdgeItem*> childEdges;

    RESIZE_TYPE resizeMode;    
};

#endif // NODEITEM_H
