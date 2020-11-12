//
// Created by Cathlyn Aston on 5/11/20.
//

#include "portinstance.h"
#include "../../../theme.h"

#include <QPainter>

using namespace Pulse::View;

/**
 * @brief PortInstance::PortInstance
 * @param label
 * @param kind
 * @param meta_label
 * @param parent
 */
PortInstance::PortInstance(const QString& label,
                           const AggServerResponse::Port::Kind kind,
                           const QString& meta_label,
                           QGraphicsItem* parent)
    : QGraphicsWidget(parent),
      kind_(kind)
{
    auto icon_name = "";
    auto icon_pos = NamePlate::IconPos::Left;

    switch (kind) {
        case AggServerResponse::Port::Kind::PERIODIC:
            icon_name = "clockCycle";
            break;
        case AggServerResponse::Port::Kind::PUBLISHER:
            icon_name = "arrowRightLong";
            icon_pos = NamePlate::IconPos::Right;
            break;
        case AggServerResponse::Port::Kind::SUBSCRIBER:
            icon_name = "arrowIntoBox";
            break;
        case AggServerResponse::Port::Kind::REQUESTER:
            icon_name = "arrowTopRight";
            icon_pos = NamePlate::IconPos::Right;
            break;
        case AggServerResponse::Port::Kind::REPLIER:
            icon_name = "arrowBottomRight";
            break;
        case AggServerResponse::Port::Kind::NO_KIND:
            throw std::invalid_argument("PortInstanceGraphicsItem::PortInstanceGraphicsItem - Port kind is unknown");
    }

    ellipse_color_ = Theme::theme()->getSeverityColor(Notification::Severity::ERROR);
    if (icon_pos == NamePlate::IconPos::Right) {
        ellipse_color_ = Theme::theme()->getSeverityColor(Notification::Severity::SUCCESS);
    }

    name_plate_ = new NamePlate(label, "Icons", icon_name, "Message", "Icons", "envelopeTwoTone", this);
    name_plate_->setIconPos(icon_pos);
    name_plate_->setPrimarySpacing(Defaults::layout_padding);

    auto icon_size = Defaults::primary_icon_size * 0.75;
    name_plate_->setPrimaryIconSize(icon_size.width(), icon_size.height());

    auto layout = new QGraphicsLinearLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addItem(name_plate_);

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    // Connect the theme to update the ellipse color
    connect(Theme::theme(), &Theme::theme_Changed, [icon_pos, this]() {
        ellipse_color_ = Theme::theme()->getSeverityColor(Notification::Severity::ERROR);
        if (icon_pos == NamePlate::IconPos::Right) {
            ellipse_color_ = Theme::theme()->getSeverityColor(Notification::Severity::SUCCESS);
        }
        update();
    });
}

/**
 * @brief PortInstance::connectModelData
 * @param model_data
 */
void PortInstance::connectModelData(QPointer<Pulse::Model::Entity> model_data)
{
    if (model_data.isNull()) {
        throw std::invalid_argument("DefaultEntity - The model data is null");
    }
    connect(model_data, &Pulse::Model::Entity::destroyed, this, &PortInstance::onModelDeleted);
    connect(model_data, &Pulse::Model::Entity::labelChanged, name_plate_, &NamePlate::changeLabel);
    connect(model_data, &Pulse::Model::Entity::iconChanged, name_plate_, &NamePlate::changeIcon);
}

/**
 * @brief PortInstance::onModelDeleted
 */
void PortInstance::onModelDeleted()
{
    deleteLater();
}

/**
 * @brief PortInstance::getAsGraphicsWidget
 * @return
 */
QGraphicsWidget* PortInstance::getAsGraphicsWidget()
{
    return this;
}

/**
 * @brief PortInstance::getKind
 * @return
 */
AggServerResponse::Port::Kind PortInstance::getKind() const
{
    return kind_;
}

/**
 * @brief PortInstance::boundingRect
 * @return
 */
QRectF PortInstance::boundingRect() const
{
    return name_plate_->boundingRect();
}

/**
 * @brief PortInstance::paint
 * @param painter
 * @param option
 * @param widget
 */
void PortInstance::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(ellipse_color_);

    auto ellipse_geom = name_plate_->getIconGeometry();
    painter->drawEllipse(ellipse_geom.center(), ellipse_geom.height() / 2.0, ellipse_geom.height() / 2.0);
}