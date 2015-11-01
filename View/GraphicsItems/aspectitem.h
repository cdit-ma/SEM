#ifndef ASPECTITEM_H
#define ASPECTITEM_H
#include "nodeitem.h"
#include "entityitem.h"
#include <QFont>

class AspectItem : public NodeItem
{
    Q_OBJECT

public:
    enum ASPECT_POS{AP_TOPLEFT, AP_TOPRIGHT, AP_BOTRIGHT, AP_BOTLEFT};

    AspectItem(Node *node, GraphMLItem *parent, AspectItem::ASPECT_POS position);
    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    qreal getWidth();
    qreal getHeight();
    void setPos(const QPointF pos);
    void setWidth(qreal width);
    void setHeight(qreal height);

private:

    QPointF getAspectPos();
    ASPECT_POS aspectPos;
    QString aspectLabel;
    QString aspectKind;

    qreal minimumHeight;
    qreal minimumWidth;
    qreal width;
    qreal height;
    QColor backgroundColor;
    QColor textColor;
    QFont textFont;

public slots:
    void graphMLDataChanged(GraphMLData *);
    void aspectsChanged(QStringList aspects);

    void sizeChanged();

public:
     QRectF gridRect() const;

    // QGraphicsItem interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    // NodeItem interface
public slots:
    void childPositionChanged();
    void childSizeChanged();
    void childUpdated();

    // NodeItem interface
public:
    qreal getItemMargin() const;
};

#endif // ASPECTITEM_H
