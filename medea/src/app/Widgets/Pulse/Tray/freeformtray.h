//
// Created by Cathlyn Aston on 29/10/20.
//

#ifndef PULSE_VIEW_FREEFORMTRAY_H
#define PULSE_VIEW_FREEFORMTRAY_H

#include "tray.h"

namespace Pulse::View {

class FreeFormTray : public QGraphicsWidget, public Tray {
public:
    explicit FreeFormTray(QGraphicsItem* parent = nullptr);
    ~FreeFormTray() override = default;

    void addItem(QGraphicsWidget* widget) override;

    [[nodiscard]] QRectF boundingRect() const override;

public slots:
    void validateItemMove(QGraphicsWidget* widget, const QPointF& pos);

private:
    [[nodiscard]] static QPointF getContentOrigin();
    [[nodiscard]] QPointF getNextStackPos() const;
    [[nodiscard]] QRectF getVisibleItemsRect() const;

    void checkPreCondition(QGraphicsWidget* widget) const;

    std::vector<QGraphicsWidget*> contained_items_;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_FREEFORMTRAY_H
