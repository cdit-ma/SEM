//
// Created by Cathlyn Aston on 29/10/20.
//

#ifndef PULSE_VIEW_TRICOLUMNTRAY_H
#define PULSE_VIEW_TRICOLUMNTRAY_H

#include "tray.h"

#include <QGraphicsGridLayout>

namespace Pulse::View {

class TriColumnTray : public QGraphicsWidget, public Tray {
public:
    explicit TriColumnTray(QGraphicsItem* parent = nullptr);
    ~TriColumnTray() override = default;

    bool isEmpty() const override;
    void addItem(QGraphicsWidget* widget) override;

    void addLeft(QGraphicsWidget* widget);
    void addRight(QGraphicsWidget* widget);
    void addCenter(QGraphicsWidget* widget);

    [[nodiscard]] QRectF boundingRect() const override;

protected:
    void setGeometry(const QRectF& geom) override;

private:
    void addWidget(QGraphicsWidget* widget, int row, int column);
    void checkPreConditions(QGraphicsWidget* widget) const;

    void constructGridLayout();

    QGraphicsGridLayout* grid_layout_ = nullptr;

    int left_count_ = 0;
    int center_count_ = 0;
    int right_count_ = 0;

    static const int left_column_ = 0;
    static const int center_column_ = 1;
    static const int right_column_ = 2;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_TRICOLUMNTRAY_H