//
// Created by Cathlyn Aston on 28/10/20.
//

#ifndef PULSE_VIEW_DEFAULTENTITYCONTAINER_H
#define PULSE_VIEW_DEFAULTENTITYCONTAINER_H

#include "entitycontainer.h"
#include "../Entity/entity.h"
#include "../NamePlate/nameplate.h"
#include "../Tray/freeformtray.h"

#include <QPen>

namespace Pulse::View {

class DefaultEntityContainer : public QGraphicsWidget, public Entity, public EntityContainer {
public:
    explicit DefaultEntityContainer(const QString& label,
                                    const QString& icon_path,
                                    const QString& icon_name,
                                    const QString& meta_label,
                                    const QString& meta_icon_path,
                                    const QString& meta_icon_name,
                                    QGraphicsItem* parent = nullptr);

    ~DefaultEntityContainer() override = default;

    void connectModelData(QPointer<Pulse::Model::Entity> model_data) override;
    void onModelDeleted() override;

    QGraphicsWidget* getAsGraphicsWidget() override;

    void add(Entity* entity) override;
    void remove(Entity* entity) override {};

    void expand() override;
    void contract() override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    [[nodiscard]] QRectF boundingRect() const override;

    void setPrimaryIconSize(int width, int height);

protected:
    void setGeometry(const QRectF& geom) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void themeChanged();

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    [[nodiscard]] QRectF topRect() const;
    [[nodiscard]] QRectF contractedRect() const;

    [[nodiscard]] bool inContractedState() const;

    [[nodiscard]] qreal getWidth() const;
    [[nodiscard]] qreal getHeight() const;

    QColor top_color_;
    QColor tray_color_;
    QPen border_pen_;

    bool mouse_pressed_ = false;
    bool top_rect_pressed_ = false;

    NamePlate* name_plate_ = nullptr;
    FreeFormTray* tray_ = nullptr;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_DEFAULTENTITYCONTAINER_H