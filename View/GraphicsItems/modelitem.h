#ifndef MODELITEM_H
#define MODELITEM_H
#include "graphmlitem.h"
#include "inputitem.h"
#include "../../Model/node.h"
#include "../../Model/graphmldata.h"
#include <QPen>

#define MODEL_WIDTH 72
#define MODEL_HEIGHT 72

#define LABEL_RATIO (1.0 / 9.0)

class ModelItem : public GraphMLItem
{
    Q_OBJECT
public:
    ModelItem(Node* node, bool IN_SUBVIEW=false);

    void adjustPos(QPointF delta);

    void setPos(const QPointF &pos);
    void setPos(qreal x, qreal y);

    QRectF boundingRect() const;
    QRectF topInputRect() const;
    QRectF bottomInputRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public slots:
    void graphMLDataChanged(GraphMLData *data);
    void dataEditModeRequested();
    void dataChanged(QString data);

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
