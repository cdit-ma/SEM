#ifndef NODEITEM_H
#define NODEITEM_H

#include "graphmlitem.h"
#include "../../Model/node.h"

class EdgeItem;
class NodeView;

class NodeItem : public GraphMLItem
{
    Q_OBJECT
public:
    enum RESIZE_TYPE{NO_RESIZE, RESIZE, HORIZONTAL_RESIZE, VERTICAL_RESIZE};

    NodeItem(Node *node, GraphMLItem *parent, GUI_KIND kind);
    ~NodeItem();

    QRectF childrenBoundingRect();
    QRectF getChildBoundingRect();

    virtual QRectF gridRect() const = 0;
    virtual QRectF minimumRect() const;
    QPointF getMinimumRectCenter() const;
    QPointF getMinimumRectCenterPos() const;
    void setMinimumRectCenterPos(QPointF pos);

    virtual void setPos(const QPointF pos) =0;
    virtual qreal getItemMargin() const = 0;


    void updatePositionInModel();
    void updateSizeInModel();

    void snapToGrid();



    void adjustSize(QSizeF delta);
    virtual void setWidth(qreal w)=0;
    virtual void setHeight(qreal h)=0;
    virtual qreal getWidth()=0;
    virtual qreal getHeight()=0;

    void showChildGridOutline(NodeItem* item, QPointF gridPoint);
    void hideChildGridOutline(int ID);
    QVector<QRectF> getChildrenGridOutlines();
    void clearChildrenGridOutlines();
    void adjustPos(QPointF delta);



    QPointF getNextChildPos(QRectF childRect = QRectF());
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
    void move(QPointF delta);
    void resize(QSizeF delta);
    void moveFinished();
    void resizeFinished();

public slots:
    void toggleGridMode(bool enabled);
    void graphMLDataChanged(GraphMLData *data) = 0;
    virtual void childPositionChanged() = 0;
    virtual void childSizeChanged() = 0;

    void updateGrid();

private:
    bool IS_GRID_MODE_ON;
    bool DRAW_GRID;

    QVector<QLineF> verticalGridLines;
    QVector<QLineF> horizontalGridLines;

    QHash<int, QRectF> childrenGridOutlines;

    RESIZE_TYPE resizeMode;
    qreal width;
    qreal height;
};

#endif // NODEITEM_H
