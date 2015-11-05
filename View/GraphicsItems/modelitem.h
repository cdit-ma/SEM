#ifndef MODELITEM_H
#define MODELITEM_H
#include "graphmlitem.h"
#include "inputitem.h"
#include "../../Model/node.h"
#include "../../enumerations.h"
#include "../../Model/graphmldata.h"
#include <QPen>

#define MODEL_WIDTH 72
#define MODEL_HEIGHT 72

#define LABEL_RATIO (1.0 / 9.0)

class ModelItem : public GraphMLItem
{
    Q_OBJECT
public:
    ModelItem(Node* node, NodeView* view);

    void adjustPos(QPointF delta);

    void setPos(const QPointF &pos);
    void setPos(qreal x, qreal y);

    QRectF boundingRect() const;
    QRectF topInputRect() const;
    QRectF bottomInputRect() const;

    QList<VIEW_ASPECT> getVisibleAspects();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setInSubView(bool inSubview);
public slots:
    void graphMLDataChanged(GraphMLData *data);
    void dataEditModeRequested();
    void dataChanged(QString data);
    void themeChanged(VIEW_THEME theme);

private:
    void setupGraphMLDataConnections();
    void setupInputItems();

    InputItem* topInputItem;
    InputItem* bottomInputItem;
    qreal width;
    qreal height;

    QColor modelCircleColor;
    QColor topLeftColor;
    QColor topRightColor;
    QColor bottomLeftColor;
    QColor bottomRightColor;
    QPen pen;
    QPen selectedPen;

    // QGraphicsItem interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

#endif // MODELITEM_H
