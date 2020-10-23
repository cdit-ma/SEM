//
// Created by Cathlyn Aston on 24/9/20.
//

#ifndef PULSE_VIEW_TEXTITEM_H
#define PULSE_VIEW_TEXTITEM_H

#include <QGraphicsTextItem>
#include <QGraphicsLayoutItem>

namespace Pulse::View {

class TextItem : public QGraphicsTextItem, public QGraphicsLayoutItem {
public:
    explicit TextItem(const QString& text = "", QGraphicsItem* parent = nullptr);

    void setText(const QString& text);
    void setAlignment(Qt::Alignment alignment);

protected:
    void setGeometry(const QRectF& geom) override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const override;

private:
    QPointF getAlignedPos() const;
    Qt::Alignment alignment_ = Qt::AlignVCenter | Qt::AlignLeft;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_TEXTITEM_H
