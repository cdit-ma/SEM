//
// Created by Cathlyn Aston on 5/11/20.
//

#ifndef PULSE_VIEW_PORTINSTANCE_H
#define PULSE_VIEW_PORTINSTANCE_H

#include "entity.h"
#include "../Edge/connectable.h"
#include "../NamePlate/nameplate.h"
#include "../EdgeAnchor/naturalanchor.h"
#include "../../../Widgets/Charts/Data/protomessagestructs.h"

#include <QPointer>

namespace Pulse::View {

class PortInstance final : public QGraphicsWidget, public Entity, public Connectable {
    Q_OBJECT

public:
    explicit PortInstance(const QString& label,
                          const AggServerResponse::Port::Kind kind,
                          const QString& meta_label = "Message",
                          QGraphicsItem* parent = nullptr);

    ~PortInstance() final = default;

    void connectModelData(QPointer<Pulse::Model::Entity> model_data) override;
    void onModelDeleted() override;

    void connectEdge(Edge* edge);
    void disconnectEdges();

    NaturalAnchor* getInputAnchor() override;
    NaturalAnchor* getOutputAnchor() override;

    QGraphicsWidget* getAsGraphicsWidget() override;

    [[nodiscard]] QRectF boundingRect() const override;

    [[nodiscard]] AggServerResponse::Port::Kind getKind() const;

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    NamePlate* name_plate_ = nullptr;

    NaturalAnchor* input_anchor_ = nullptr;
    NaturalAnchor* output_anchor_ = nullptr;

    QColor ellipse_color_;
    AggServerResponse::Port::Kind kind_;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_PORTINSTANCE_H