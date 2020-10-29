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

    void add(QGraphicsWidget* widget) override;

    [[nodiscard]] QRectF boundingRect() const override;

public slots:
    void validateContentMove(QGraphicsWidget* widget, const QPointF& pos);

private:
    [[nodiscard]] static QPointF getContentOrigin();
    [[nodiscard]] QPointF getContentStackPos() const;

    std::vector<QGraphicsWidget*> content_widgets_;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_FREEFORMTRAY_H
