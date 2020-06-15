#ifndef TEXTGRAPHICSITEM_H
#define TEXTGRAPHICSITEM_H

#include "graphicslayoutitem.h"
#include <QGraphicsTextItem>

class TextGraphicsItem : public QGraphicsTextItem, public MEDEA::GraphicsLayoutItem
{
public:
    explicit TextGraphicsItem(const QString &text, QGraphicsItem* parent = nullptr);

    void setTextAlignment(Qt::Alignment alignment);

protected:
    void setGeometry(const QRectF &geom) override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const override;

private:
    QPointF getAlignedPos() const;
    Qt::Alignment alignment_ = Qt::AlignVCenter | Qt::AlignLeft;
};

#endif // TEXTGRAPHICSITEM_H