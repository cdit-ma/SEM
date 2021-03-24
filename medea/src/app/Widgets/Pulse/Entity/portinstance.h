//
// Created by Cathlyn Aston on 5/11/20.
//

#ifndef PULSE_VIEW_PORTINSTANCE_H
#define PULSE_VIEW_PORTINSTANCE_H

#include "entity.h"
#include "../Edge/connectable.h"
#include "../NamePlate/nameplate.h"
#include "../EdgeAnchor/naturalanchor.h"
#include "../../Charts/Data/portinstancedata.h"
#include "../../../Widgets/Charts/Data/protomessagestructs.h"

#include <QPointer>
#include <QPen>

namespace Pulse::View {

class PortInstance final : public QGraphicsWidget, public Entity, public Connectable {
    Q_OBJECT

public:
    explicit PortInstance(const QPointer<PortInstanceData>& port_data, QGraphicsItem* parent = nullptr);
    ~PortInstance() final = default;

    void connectModelData(QPointer<Pulse::Model::Entity> model_data) override;
    void onModelDeleted() override;

    NaturalAnchor* getInputAnchor() override;
    NaturalAnchor* getOutputAnchor() override;

    QGraphicsWidget* getAsGraphicsWidget() override;

    [[nodiscard]] QRectF boundingRect() const override;
    [[nodiscard]] AggServerResponse::Port::Kind getKind() const;

    qint64 getPreviousEventTime(qint64 time) const;
    qint64 getNextEventTime(qint64 time) const;

    void flashEvents(qint64 from_time, qint64 to_time);

signals:
    void flashEdge(qint64 from_time, qint64 flash_duration);

    void flashedPortLifecycle();
    void unflashedPortLifecycle();

    void flashedPortEvent();
    void unflashedPortEvent();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    void themeChanged();

    void flashPort(MEDEA::ChartDataKind event_kind, qint64 from_time);
    void unflashPort(MEDEA::ChartDataKind event_kind, qint64 flash_end_time);

    bool isEventSourcePort() const;

    // These are used to prevent the flash from being stopped/reset prematurely due to previous flash timers ending
    qint64 port_lifecycle_flash_end_time_ = 0;
    qint64 port_event_flash_end_time_ = 0;

    NamePlate* name_plate_ = nullptr;

    NaturalAnchor* input_anchor_ = nullptr;
    NaturalAnchor* output_anchor_ = nullptr;

    QColor active_ellipse_color_;
    QColor default_ellipse_color_;
    QColor highlight_ellipse_color_;

    QPen ellipse_pen_;
    QColor ellipse_pen_color_;

    QPointer<PortInstanceData> port_inst_data_;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_PORTINSTANCE_H