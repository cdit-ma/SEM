//
// Created by Cathlyn Aston on 29/10/20.
//

#ifndef PULSE_VIEW_FREEFORMTRAY_H
#define PULSE_VIEW_FREEFORMTRAY_H

#include "tray.h"

namespace Pulse::View {

/**
 * @brief This Tray implementation allows its contained QGraphicsWidgets to be moved around freely inside it
 * It also inherits QGraphicsWidget which means that it can be added into a QGraphicsLayout
 */
class FreeFormTray : public QGraphicsWidget, public Tray {
public:
    explicit FreeFormTray(QGraphicsItem* parent = nullptr);
    ~FreeFormTray() override = default;

    void addItem(QGraphicsWidget* widget) override;
    void removeItem(QGraphicsWidget* widget) override;

    [[nodiscard]] bool isEmpty() const override;
    [[nodiscard]] QRectF boundingRect() const override;

protected:
    void setGeometry(const QRectF& geom) override;

private:
    [[nodiscard]] QPointF getNextStackPos() const;

    void checkPreConditions(QGraphicsWidget* widget) const;

    QList<QGraphicsWidget*> contained_items_;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_FREEFORMTRAY_H