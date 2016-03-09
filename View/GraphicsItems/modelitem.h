#ifndef MODELITEM_H
#define MODELITEM_H
#include "graphmlitem.h"
#include "inputitem.h"
#include "../../Model/node.h"
#include "../../enumerations.h"
#include "../../Model/data.h"
#include "../../Controller/nodeadapter.h"
#include <QPen>

#define MODEL_WIDTH 72
#define MODEL_HEIGHT 72

#define LABEL_RATIO (1.0 / 9.0)

class ModelItem : public GraphMLItem
{
    Q_OBJECT
public:
    ModelItem(NodeAdapter* node, NodeView* view);
    ~ModelItem();

    void adjustPos(QPointF delta);

    void setPos(const QPointF &pos);
    void setPos(qreal x, qreal y);

    QRectF boundingRect() const;
    QRectF centerInputRect() const;

    QList<VIEW_ASPECT> getVisibleAspects();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setInSubView(bool inSubview);
public slots:
    void dataChanged(QString keyName, QVariant data);
    void dataEditModeRequested();
    void dataChanged(QString data);
    void themeChanged();

private:
    void setupDataConnections();
    void setupInputItems();

    InputItem* middlewareItem;
    qreal width;
    qreal height;

    QColor modelCircleColor;
    QColor rectColor;
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
