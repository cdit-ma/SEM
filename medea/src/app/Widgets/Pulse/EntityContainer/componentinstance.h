//
// Created by Cathlyn Aston on 5/11/20.
//

#ifndef PULSE_VIEW_COMPONENTINSTANCE_H
#define PULSE_VIEW_COMPONENTINSTANCE_H

#include "entitycontainer.h"
#include "../Entity/entity.h"
#include "../NamePlate/nameplate.h"
#include "../Tray/tricolumntray.h"

#include <QPen>

namespace Pulse::View {

class ComponentInstance : public QGraphicsWidget, public Entity, public EntityContainer {
public:
    explicit ComponentInstance(const QString& label,
                               const QString& meta_label,
                               QGraphicsItem* parent = nullptr);

    ~ComponentInstance() override = default;

    void connectModelData(QPointer<Pulse::Model::Entity> model_data) override;
    void onModelDeleted() override;

    QGraphicsWidget* getAsGraphicsWidget() override;

    void add(Entity* entity) override;
    void remove(Entity* entity) override {};

    void expand() override;
    void contract() override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    [[nodiscard]] QRectF boundingRect() const override;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void themeChanged();

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    [[nodiscard]] QRectF topRect() const;
    [[nodiscard]] QRectF contractedRect() const;

    [[nodiscard]] bool inContractedState() const;

    [[nodiscard]] qreal getWidth() const;
    [[nodiscard]] qreal getHeight() const;

    void setGeometry(const QRectF& rect) override;

    QColor top_color_;
    QColor tray_color_;
    QPen border_pen_;

    bool mouse_pressed_ = false;
    bool top_rect_pressed_ = false;

    NamePlate* name_plate_ = nullptr;
    TriColumnTray* tray_ = nullptr;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_COMPONENTINSTANCE_H